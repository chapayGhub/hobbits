#include "httpdata.h"

HttpData::HttpData() :
    http(nullptr)
{

}

HttpData::~HttpData()
{
    if (http) {
        delete http;
    }
}

ImportExportInterface* HttpData::createDefaultImporterExporter()
{
    return new HttpData();
}

QString HttpData::getName()
{
    return "HTTP Data (REST)";
}

bool HttpData::canExport()
{
    return true;
}

bool HttpData::canImport()
{
    return true;
}

QString HttpData::getImportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("url")) {
        QString url = pluginState.value("url").toString();
        return QString("Import from %1").arg(url);
    }
    return "";
}

QString HttpData::getExportLabelForState(QJsonObject pluginState)
{
    return "";
}

QSharedPointer<ImportExportResult> HttpData::importBits(QJsonObject pluginState, QWidget *parent)
{
    if (!http) {
        http = new HttpTransceiver(parent);
    }
    http->setDownloadMode();

    if (pluginState.contains("url")) {
        http->setUrl(QUrl(pluginState.value("url").toString()));
    }

    if (http->exec()) {
        auto container = QSharedPointer<BitContainer>(new BitContainer());
        container->setBits(http->getDownloadedData());
        container->setName(http->getUrl().toDisplayString());

        pluginState.remove("url");
        pluginState.insert("url", http->getUrl().toDisplayString());

        return ImportExportResult::create(container, pluginState);
    }

    return ImportExportResult::nullResult();
}

QSharedPointer<ImportExportResult> HttpData::exportBits(QSharedPointer<const BitContainer> container, QJsonObject pluginState, QWidget *parent)
{
    if (!http) {
        http = new HttpTransceiver(parent);
    }
    http->setUploadMode(container->bits()->getPreviewBytes());

    if (pluginState.contains("url")) {
        http->setUrl(QUrl(pluginState.value("url").toString()));
    }

    http->exec();

    pluginState.remove("url");
    pluginState.insert("url", http->getUrl().toDisplayString());

    return ImportExportResult::create(pluginState);
}
