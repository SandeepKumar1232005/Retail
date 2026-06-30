import sqlite3
import random

def insert_data():
    conn = sqlite3.connect('retail_ms.db')
    cursor = conn.cursor()

    # Insert categories
    categories = [
        ('Electronics', 'Electronic items and gadgets'),
        ('Groceries', 'Daily grocery items'),
        ('Clothing', 'Apparel and accessories'),
        ('Furniture', 'Home and office furniture'),
        ('Stationery', 'Office and school supplies'),
        ('Toys', 'Kids toys and games'),
        ('Beauty', 'Cosmetics and personal care'),
        ('Sports', 'Sports equipment and apparel'),
        ('Automotive', 'Car and bike accessories'),
        ('Books', 'Books and magazines')
    ]

    cursor.executemany("INSERT OR IGNORE INTO categories (name, description) VALUES (?, ?)", categories)

    # Fetch inserted category IDs
    cursor.execute("SELECT id, name FROM categories")
    category_map = {row[1]: row[0] for row in cursor.fetchall()}

    # Insert products
    products = [
        ('Smartphone X1', 'SKU-ELEC-001', '890123456001', category_map['Electronics'], 15000, 18000, 20000, 10, 18, 0, 'pcs', 50, 10, 100, 'High-end smartphone'),
        ('Laptop Pro', 'SKU-ELEC-002', '890123456002', category_map['Electronics'], 45000, 52000, 55000, 5, 18, 0, 'pcs', 30, 5, 50, 'Professional laptop'),
        ('Wireless Earbuds', 'SKU-ELEC-003', '890123456003', category_map['Electronics'], 1200, 1999, 2499, 20, 18, 0, 'pcs', 100, 20, 200, 'Noise cancelling earbuds'),
        ('Organic Basmati Rice', 'SKU-GROC-001', '890123456004', category_map['Groceries'], 80, 110, 120, 5, 5, 1, 'kg', 500, 100, 1000, 'Premium basmati rice'),
        ('Whole Wheat Flour', 'SKU-GROC-002', '890123456005', category_map['Groceries'], 30, 42, 45, 0, 0, 1, 'kg', 1000, 200, 2000, 'Chakki fresh atta'),
        ('Olive Oil', 'SKU-GROC-003', '890123456006', category_map['Groceries'], 600, 850, 999, 15, 5, 0, 'ltr', 150, 20, 300, 'Extra virgin olive oil'),
        ('Men\'s Cotton T-Shirt', 'SKU-CLOT-001', '890123456007', category_map['Clothing'], 200, 499, 699, 25, 12, 0, 'pcs', 200, 30, 500, '100% cotton t-shirt'),
        ('Women\'s Denim Jeans', 'SKU-CLOT-002', '890123456008', category_map['Clothing'], 450, 999, 1499, 30, 12, 0, 'pcs', 150, 20, 400, 'Slim fit denim jeans'),
        ('Office Chair', 'SKU-FURN-001', '890123456009', category_map['Furniture'], 2500, 4500, 5999, 25, 18, 0, 'pcs', 40, 5, 100, 'Ergonomic office chair'),
        ('Study Table', 'SKU-FURN-002', '890123456010', category_map['Furniture'], 1800, 3200, 4000, 20, 18, 0, 'pcs', 30, 5, 80, 'Wooden study table'),
        ('Notebook A4', 'SKU-STAT-001', '890123456011', category_map['Stationery'], 40, 60, 75, 20, 12, 1, 'pcs', 400, 50, 1000, '200 pages single rule notebook'),
        ('Gel Pens Pack', 'SKU-STAT-002', '890123456012', category_map['Stationery'], 30, 50, 60, 15, 12, 1, 'pcs', 250, 30, 600, 'Pack of 5 blue gel pens'),
        ('Lego Building Blocks', 'SKU-TOYS-001', '890123456013', category_map['Toys'], 800, 1299, 1599, 15, 18, 0, 'pcs', 60, 10, 150, 'Classic building blocks set'),
        ('Moisturizing Cream', 'SKU-BEAU-001', '890123456014', category_map['Beauty'], 150, 250, 299, 15, 18, 1, 'pcs', 120, 20, 300, 'Daily moisturizing cream'),
        ('Shampoo 500ml', 'SKU-BEAU-002', '890123456015', category_map['Beauty'], 180, 280, 349, 20, 18, 1, 'pcs', 180, 30, 400, 'Anti-dandruff shampoo'),
        ('Yoga Mat', 'SKU-SPOR-001', '890123456016', category_map['Sports'], 300, 599, 899, 30, 12, 0, 'pcs', 100, 15, 250, 'Anti-slip 6mm yoga mat'),
        ('Car Wash Shampoo', 'SKU-AUTO-001', '890123456017', category_map['Automotive'], 120, 199, 249, 20, 18, 0, 'ltr', 80, 10, 200, 'High foam car wash shampoo'),
        ('Fiction Novel', 'SKU-BOOK-001', '890123456018', category_map['Books'], 150, 250, 299, 15, 5, 1, 'pcs', 70, 10, 150, 'Bestselling fiction novel'),
        ('Smart Watch', 'SKU-ELEC-004', '890123456019', category_map['Electronics'], 2000, 3499, 4999, 30, 18, 0, 'pcs', 80, 15, 200, 'Fitness tracker smart watch'),
        ('Running Shoes', 'SKU-SPOR-002', '890123456020', category_map['Sports'], 800, 1499, 2499, 40, 12, 0, 'pcs', 120, 20, 300, 'Lightweight running shoes')
    ]

    query = """
    INSERT INTO products (
        name, sku, barcode, category_id, cost_price, selling_price, mrp, 
        discount_pct, gst_rate, is_gst_inclusive, unit, stock_quantity, 
        min_stock, max_stock, description
    ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    ON CONFLICT(sku) DO UPDATE SET
        name=excluded.name,
        barcode=excluded.barcode,
        category_id=excluded.category_id,
        cost_price=excluded.cost_price,
        selling_price=excluded.selling_price,
        mrp=excluded.mrp,
        discount_pct=excluded.discount_pct,
        gst_rate=excluded.gst_rate,
        is_gst_inclusive=excluded.is_gst_inclusive,
        unit=excluded.unit,
        stock_quantity=excluded.stock_quantity,
        min_stock=excluded.min_stock,
        max_stock=excluded.max_stock,
        description=excluded.description
    """
    
    cursor.executemany(query, products)
    
    conn.commit()
    print(f"Inserted {cursor.rowcount} products and some categories.")
    conn.close()

if __name__ == '__main__':
    insert_data()
