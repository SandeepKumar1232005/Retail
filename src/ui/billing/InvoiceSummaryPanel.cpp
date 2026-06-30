#include "InvoiceSummaryPanel.h"
#include <QVBoxLayout>
#include <QDateTime>

namespace RetailMS {

InvoiceSummaryPanel::InvoiceSummaryPanel(QWidget* parent) : QDialog(parent) {
    setupUi();
}

void InvoiceSummaryPanel::setupUi() {
    setWindowTitle("Invoice Preview");
    setMinimumSize(400, 600);
    setStyleSheet("QDialog { background-color: #121212; }");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);

    QWidget* frame = new QWidget(this);
    frame->setObjectName("receiptCard");
    frame->setStyleSheet("QWidget#receiptCard { background-color: #1E2025; border-radius: 8px; padding: 10px; }");
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);

    QLabel* title = new QLabel("🧾 Invoice Preview", frame);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF; margin-bottom: 5px;");
    frameLayout->addWidget(title);

    m_receiptText = new QTextEdit(frame);
    m_receiptText->setReadOnly(true);
    m_receiptText->setStyleSheet(
        "QTextEdit { background-color: #FAFAFA; color: #111111; font-family: 'Courier New', Courier, monospace; font-size: 13px; padding: 15px; border-radius: 4px; border: none; }"
    );
    frameLayout->addWidget(m_receiptText);

    layout->addWidget(frame);
}

void InvoiceSummaryPanel::refreshSummary(const Invoice& invoice) {
    QString cashier = "Admin"; // In a real app, this comes from SessionManager
    QString customerName = invoice.customerId > 0 ? QString("%1 (CUS-%2)").arg(invoice.customerName.isEmpty() ? QString("Walk-in") : invoice.customerName).arg(invoice.customerId, 6, 10, QChar('0')) : "Walk-in Customer";
    
    QString receipt = "<div style='text-align:center;'>"
                      "<h2 style='margin:0;'>RETAIL SUPERMARKET</h2>"
                      "<p style='margin:0; font-size:11px;'>123 Main Street, City</p>"
                      "<p style='margin:0; font-size:11px;'>Tel: +1 234 567 8900</p>"
                      "</div>"
                      "<hr style='border: 1px dashed #CCC; margin-top: 10px; margin-bottom: 10px;'>";
                      
    receipt += QString("<table width='100%' style='font-size: 12px;'>"
                       "<tr><td><b>Invoice No:</b> %1</td><td align='right'><b>Date:</b> %2</td></tr>"
                       "<tr><td><b>Cashier:</b> %3</td><td align='right'><b>Time:</b> %4</td></tr>"
                       "<tr><td colspan='2'><b>Customer:</b> %5</td></tr>"
                       "</table>"
                       "<hr style='border: 1px dashed #CCC;'>")
               .arg(invoice.invoiceNumber)
               .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy"))
               .arg(cashier)
               .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
               .arg(customerName);
    
    receipt += "<table width='100%' style='font-size: 12px;'>"
               "<tr><th align='left' style='border-bottom: 1px solid #CCC;'>Item</th>"
               "<th align='center' style='border-bottom: 1px solid #CCC;'>Qty</th>"
               "<th align='right' style='border-bottom: 1px solid #CCC;'>Total</th></tr>";
               
    for (const auto& item : invoice.items) {
        receipt += QString("<tr><td style='padding-top: 5px;'>%1<br><small style='color:#666;'>@ %2</small></td>"
                           "<td align='center' style='padding-top: 5px;'>%3</td>"
                           "<td align='right' style='padding-top: 5px;'>%4</td></tr>")
                   .arg(item.productName)
                   .arg(item.sellingPrice, 0, 'f', 2)
                   .arg(item.quantity)
                   .arg(item.total, 0, 'f', 2);
    }
    receipt += "</table><hr style='border: 1px dashed #CCC;'>";
    
    receipt += QString("<table width='100%' style='font-size: 12px;'>"
                       "<tr><td>Subtotal:</td><td align='right'>%1</td></tr>"
                       "<tr><td>Discount:</td><td align='right'>-%2</td></tr>"
                       "<tr><td>Tax (GST):</td><td align='right'>+%3</td></tr>"
                       "<tr><td colspan='2'><hr style='border: 1px solid #CCC;'></td></tr>"
                       "<tr><th style='font-size: 14px;'>GRAND TOTAL:</th><th align='right' style='font-size: 14px;'>%4</th></tr>"
                       "</table>")
               .arg(invoice.subtotal, 0, 'f', 2)
               .arg(invoice.discountAmt + invoice.couponDiscount, 0, 'f', 2)
               .arg(invoice.cgstAmt + invoice.sgstAmt, 0, 'f', 2)
               .arg(invoice.grandTotal, 0, 'f', 2);
               
    receipt += "<hr style='border: 1px dashed #CCC;'><div style='text-align:center; font-size: 11px;'><p>Thank you for shopping with us!<br>Please visit again.</p></div>";
    
    m_receiptText->setHtml(receipt);
}

} // namespace RetailMS
