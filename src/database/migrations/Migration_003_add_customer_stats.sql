-- Add total_orders and last_visit columns to customers table
ALTER TABLE customers ADD COLUMN total_orders INTEGER DEFAULT 0;
ALTER TABLE customers ADD COLUMN last_visit DATE;
