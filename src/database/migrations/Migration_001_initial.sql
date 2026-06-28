-- ─── USERS ───────────────────────────────────────────────
CREATE TABLE users (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    username      TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,          -- PBKDF2-SHA256
    salt          TEXT NOT NULL,
    full_name     TEXT NOT NULL,
    role          TEXT NOT NULL CHECK(role IN ('admin','cashier','manager')),
    email         TEXT,
    phone         TEXT,
    is_active     INTEGER NOT NULL DEFAULT 1,
    last_login    DATETIME,
    created_at    DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at    DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── CATEGORIES ──────────────────────────────────────────
CREATE TABLE categories (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT NOT NULL UNIQUE,
    description TEXT,
    parent_id   INTEGER REFERENCES categories(id),  -- nested categories
    icon        TEXT,
    created_at  DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── SUPPLIERS ───────────────────────────────────────────
CREATE TABLE suppliers (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    name         TEXT NOT NULL,
    contact_name TEXT,
    phone        TEXT,
    email        TEXT,
    address      TEXT,
    gst_number   TEXT,
    pan_number   TEXT,
    bank_account TEXT,
    ifsc_code    TEXT,
    credit_days  INTEGER DEFAULT 30,
    is_active    INTEGER DEFAULT 1,
    created_at   DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at   DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── PRODUCTS ────────────────────────────────────────────
CREATE TABLE products (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    name             TEXT NOT NULL,
    sku              TEXT NOT NULL UNIQUE,
    barcode          TEXT UNIQUE,
    category_id      INTEGER REFERENCES categories(id),
    supplier_id      INTEGER REFERENCES suppliers(id),
    cost_price       REAL NOT NULL DEFAULT 0,
    selling_price    REAL NOT NULL DEFAULT 0,
    mrp              REAL,
    discount_pct     REAL DEFAULT 0,
    gst_rate         REAL DEFAULT 18,        -- 0,5,12,18,28
    cgst_rate        REAL GENERATED ALWAYS AS (gst_rate / 2) VIRTUAL,
    sgst_rate        REAL GENERATED ALWAYS AS (gst_rate / 2) VIRTUAL,
    is_gst_inclusive INTEGER DEFAULT 0,
    unit             TEXT DEFAULT 'pcs',     -- pcs, kg, ltr, mtr
    stock_quantity   REAL NOT NULL DEFAULT 0,
    min_stock        REAL DEFAULT 5,
    max_stock        REAL,
    expiry_date      DATE,
    batch_number     TEXT,
    hsn_code         TEXT,
    description      TEXT,
    image_path       TEXT,
    is_active        INTEGER DEFAULT 1,
    created_at       DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at       DATETIME DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX idx_products_barcode     ON products(barcode);
CREATE INDEX idx_products_category    ON products(category_id);
CREATE INDEX idx_products_name        ON products(name COLLATE NOCASE);

-- ─── CUSTOMERS ───────────────────────────────────────────
CREATE TABLE customers (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    name           TEXT NOT NULL,
    phone          TEXT UNIQUE,
    email          TEXT,
    address        TEXT,
    city           TEXT,
    pincode        TEXT,
    gst_number     TEXT,
    loyalty_points INTEGER DEFAULT 0,
    total_spent    REAL DEFAULT 0,
    tier           TEXT DEFAULT 'silver' CHECK(tier IN ('silver','gold','platinum')),
    dob            DATE,
    anniversary    DATE,
    notes          TEXT,
    created_at     DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at     DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── EMPLOYEES ───────────────────────────────────────────
CREATE TABLE employees (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id      INTEGER REFERENCES users(id),
    employee_no  TEXT NOT NULL UNIQUE,
    department   TEXT,
    designation  TEXT,
    salary       REAL,
    join_date    DATE,
    address      TEXT,
    emergency_contact TEXT,
    created_at   DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── INVOICES ────────────────────────────────────────────
CREATE TABLE invoices (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    invoice_number  TEXT NOT NULL UNIQUE,  -- auto: INV-20240601-0001
    customer_id     INTEGER REFERENCES customers(id),
    user_id         INTEGER NOT NULL REFERENCES users(id),
    subtotal        REAL NOT NULL DEFAULT 0,
    discount_amt    REAL DEFAULT 0,
    coupon_code     TEXT,
    coupon_discount REAL DEFAULT 0,
    cgst_amt        REAL DEFAULT 0,
    sgst_amt        REAL DEFAULT 0,
    igst_amt        REAL DEFAULT 0,
    round_off       REAL DEFAULT 0,
    grand_total     REAL NOT NULL DEFAULT 0,
    amount_paid     REAL DEFAULT 0,
    change_returned REAL DEFAULT 0,
    payment_mode    TEXT NOT NULL DEFAULT 'cash'
                    CHECK(payment_mode IN ('cash','upi','card','split')),
    payment_ref     TEXT,                  -- UPI transaction ID, card last4
    status          TEXT DEFAULT 'paid'
                    CHECK(status IN ('draft','paid','refunded','voided')),
    notes           TEXT,
    loyalty_redeemed INTEGER DEFAULT 0,
    loyalty_earned   INTEGER DEFAULT 0,
    invoice_date    DATETIME DEFAULT CURRENT_TIMESTAMP,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX idx_invoices_number   ON invoices(invoice_number);
CREATE INDEX idx_invoices_date     ON invoices(invoice_date);
CREATE INDEX idx_invoices_customer ON invoices(customer_id);

-- ─── INVOICE ITEMS ───────────────────────────────────────
CREATE TABLE invoice_items (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    invoice_id   INTEGER NOT NULL REFERENCES invoices(id) ON DELETE CASCADE,
    product_id   INTEGER NOT NULL REFERENCES products(id),
    product_name TEXT NOT NULL,            -- snapshot at time of sale
    barcode      TEXT,
    quantity     REAL NOT NULL,
    unit         TEXT,
    cost_price   REAL NOT NULL,
    selling_price REAL NOT NULL,
    discount_pct  REAL DEFAULT 0,
    discount_amt  REAL DEFAULT 0,
    taxable_amt   REAL,
    cgst_rate     REAL DEFAULT 0,
    sgst_rate     REAL DEFAULT 0,
    cgst_amt      REAL DEFAULT 0,
    sgst_amt      REAL DEFAULT 0,
    total         REAL NOT NULL
);

-- ─── INVENTORY LOGS ──────────────────────────────────────
CREATE TABLE inventory_logs (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id   INTEGER NOT NULL REFERENCES products(id),
    user_id      INTEGER REFERENCES users(id),
    type         TEXT NOT NULL
                 CHECK(type IN ('sale','purchase','adjustment','return','damage','expiry')),
    quantity     REAL NOT NULL,            -- positive=in, negative=out
    before_qty   REAL NOT NULL,
    after_qty    REAL NOT NULL,
    reference_id INTEGER,                  -- invoice_id or purchase_order_id
    reference_no TEXT,
    notes        TEXT,
    created_at   DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── PURCHASE ORDERS ─────────────────────────────────────
CREATE TABLE purchase_orders (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    po_number    TEXT NOT NULL UNIQUE,
    supplier_id  INTEGER NOT NULL REFERENCES suppliers(id),
    user_id      INTEGER REFERENCES users(id),
    status       TEXT DEFAULT 'pending'
                 CHECK(status IN ('pending','ordered','received','cancelled')),
    total_amount REAL DEFAULT 0,
    notes        TEXT,
    expected_date DATE,
    received_date DATE,
    created_at   DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE purchase_order_items (
    id               INTEGER PRIMARY KEY AUTOINCREMENT,
    po_id            INTEGER NOT NULL REFERENCES purchase_orders(id) ON DELETE CASCADE,
    product_id       INTEGER NOT NULL REFERENCES products(id),
    quantity_ordered REAL NOT NULL,
    quantity_received REAL DEFAULT 0,
    cost_price       REAL NOT NULL,
    total            REAL NOT NULL
);

-- ─── EXPENSES ────────────────────────────────────────────
CREATE TABLE expenses (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    category     TEXT NOT NULL,
    description  TEXT NOT NULL,
    amount       REAL NOT NULL,
    payment_mode TEXT,
    paid_to      TEXT,
    receipt_path TEXT,
    expense_date DATE DEFAULT (date('now')),
    user_id      INTEGER REFERENCES users(id),
    created_at   DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── COUPONS ─────────────────────────────────────────────
CREATE TABLE coupons (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    code            TEXT NOT NULL UNIQUE,
    description     TEXT,
    type            TEXT NOT NULL CHECK(type IN ('flat','percent')),
    value           REAL NOT NULL,
    min_order_value REAL DEFAULT 0,
    max_discount    REAL,
    usage_limit     INTEGER,
    used_count      INTEGER DEFAULT 0,
    valid_from      DATE,
    valid_until     DATE,
    is_active       INTEGER DEFAULT 1,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── SETTINGS ────────────────────────────────────────────
CREATE TABLE settings (
    key   TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

-- ─── NOTIFICATIONS ───────────────────────────────────────
CREATE TABLE notifications (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    type       TEXT NOT NULL,              -- low_stock, expiry, backup
    message    TEXT NOT NULL,
    reference  TEXT,
    is_read    INTEGER DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── ACTIVITY LOGS ───────────────────────────────────────
CREATE TABLE activity_logs (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id    INTEGER REFERENCES users(id),
    action     TEXT NOT NULL,              -- CREATE_PRODUCT, DELETE_INVOICE, etc.
    module     TEXT NOT NULL,
    details    TEXT,                       -- JSON snapshot
    ip_address TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ─── ATTENDANCE ──────────────────────────────────────────
CREATE TABLE attendance (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    employee_id INTEGER NOT NULL REFERENCES employees(id),
    date        DATE NOT NULL,
    check_in    TIME,
    check_out   TIME,
    status      TEXT DEFAULT 'present'
                CHECK(status IN ('present','absent','half_day','leave')),
    notes       TEXT
);
