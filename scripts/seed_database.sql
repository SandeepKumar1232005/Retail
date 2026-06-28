INSERT INTO categories (name, description) VALUES 
('Electronics', 'Electronic items and accessories'),
('Groceries', 'Daily consumption items');

INSERT INTO products (barcode, name, description, category_id, cost_price, selling_price, stock_quantity) VALUES 
('10001', 'Smartphone', '5G Smartphone 128GB', 1, 300.0, 499.99, 50),
('10002', 'Headphones', 'Noise cancelling headphones', 1, 50.0, 99.99, 100),
('20001', 'Apple', 'Fresh Red Apples', 2, 0.5, 1.2, 500);

INSERT INTO customers (name, phone, tier) VALUES 
('John Doe', '1234567890', 'silver'),
('Jane Smith', '0987654321', 'gold');

INSERT INTO users (username, password_hash, salt, role) VALUES 
('admin', 'admin_hash_placeholder', 'salt_placeholder', 'admin');
