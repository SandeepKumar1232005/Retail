INSERT OR IGNORE INTO categories (name, description) VALUES 
('Electronics', 'Electronic items and accessories'),
('Groceries', 'Daily consumption items');

INSERT OR IGNORE INTO products (barcode, sku, name, description, category_id, cost_price, selling_price, stock_quantity) VALUES 
('10001', 'SKU-SMARTPHONE', 'Smartphone', '5G Smartphone 128GB', 1, 300.0, 499.99, 50),
('10002', 'SKU-HEADPHONES', 'Headphones', 'Noise cancelling headphones', 1, 50.0, 99.99, 100),
('20001', 'SKU-APPLE', 'Apple', 'Fresh Red Apples', 2, 0.5, 1.2, 500);

INSERT OR IGNORE INTO customers (name, phone, tier) VALUES 
('John Doe', '1234567890', 'silver'),
('Jane Smith', '0987654321', 'gold');

INSERT OR IGNORE INTO users (username, password_hash, salt, full_name, role) VALUES 
('admin', 'admin_hash_placeholder', 'salt_placeholder', 'Administrator', 'admin');
