#pragma once
#include <QObject>
#include <QString>

namespace RetailMS {

class BaseController : public QObject {
    Q_OBJECT
public:
    explicit BaseController(QObject* parent = nullptr);
    virtual ~BaseController() = default;

signals:
    void errorOccurred(const QString& message, const QString& title = "Error");
    void infoMessage(const QString& message, const QString& title = "Information");
    void successMessage(const QString& message, const QString& title = "Success");
    
protected:
    void handleError(const QString& context, const std::exception& e);
};

} // namespace RetailMS
