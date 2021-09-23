#include <QtNetwork>

namespace htmlPull{
    const u_int NETCODE_DATETIME_SYNCED(0);
    const u_int NETCODE_DATETIME_DESYNC(2);
    const u_int NETCODE_ERROR_TIME(3);
    const u_int NETCODE_ERROR_DATE(4);

void htmlGet(const QUrl &url, const std::function<void(const QString&,int* const result)> &fun, int* const result) {
   QScopedPointer<QNetworkAccessManager> manager(new QNetworkAccessManager);
   QNetworkReply *response = manager->get(QNetworkRequest(QUrl(url)));
   QObject::connect(response, &QNetworkReply::finished, [response, func,result,pmw]{
      response->deleteLater();
      response->manager()->deleteLater();
      if (response->error() != QNetworkReply::NoError)
      {
          *result=1;    // exit with error code 1; internet connection isn't estabilished
          return;
      }
      auto const html = QString::fromUtf8(response->readAll());
      fun(html,result); // process data
   }) && manager.take();
}

void timeSync(const QString& str, int* const result)
{
    // https://www.calendardate.com/todays.htm
    // date, time - время и дата сервера
    // Date, Time - время и дата системы
    const u_int NETDATA_OFFSET_SHIFT(8);

    const QString pTime("\\d\\d:\\d\\d:\\d\\d");
    const QString pDate("\\d\\d-\\d\\d-\\d\\d");
    QDateTime qDateTime=QDateTime().currentDateTime();

    QRegularExpression reDate(pDate);
    QRegularExpressionMatch matchDate = reDate.match(str);  // Retrieving only needed data

    QString date;
    u_int k=matchDate.capturedStart();                      // Index search in response string
    for(int j=0;
        j<NETDATA_OFFSET_SHIFT;++j) date.append(str[k+j]);

    if(QDate::fromString(date,"yy-MM-dd")==QDate())
    {
        // QDate is failed to build.
        *result=NETCODE_ERROR_DATE;
        return;
    }

    QString Date=qDateTime.date().toString("yy-MM-dd");
    QString Time=qDateTime.time().toString();
    QRegularExpression re(pTime);
    QRegularExpressionMatch match = re.match(str);

    QString time;
    u_int i=match.capturedStart();
    for(int j=0;
        j<NETDATA_OFFSET_SHIFT;++j) time.append(str[i+j]);

    if(QTime::fromString(time)==QTime())
    {
        // QTime is failed to build.
        *result=NETCODE_ERROR_TIME;
        return;
    }

    int diff=qDateTime.time().secsTo(QTime::fromString(time));

    if(Date!=date || abs(diff)>60)
    {
        // DESYNC: Time setting is incorrect
        *result=NETCODE_DATETIME_DESYNC;
    }
    else
    {   // SYNCED
        *result=NETCODE_DATETIME_SYNCED;
    }
}

void main()
{
	int* res=new int(0);
	htmlGet("https://www.calendardate.com/todays.htm",timeSync,res);
	delay(5000);
	
	switch(*res){
	case NETCODE_DATETIME_SYNCED:
	{// We're good
		
		break;
	}
	case NETCODE_DATETIME_DESYNC:
	{

		break;
	}
	case NETCODE_ERROR_TIME
	{

		break;
	}
	case NETCODE_ERROR_DATE:
	{
		
		break;
	}
	delete res;
}
