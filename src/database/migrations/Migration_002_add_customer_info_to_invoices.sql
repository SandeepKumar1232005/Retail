-- Add customer_name and customer_phone columns to freeze data in invoices table
ALTER TABLE invoices ADD COLUMN customer_name TEXT;
ALTER TABLE invoices ADD COLUMN customer_phone TEXT;
