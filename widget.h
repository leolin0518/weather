#ifndef WIDGET_H
#define WIDGET_H


#include <QtCharts>
QT_CHARTS_USE_NAMESPACE

#include <QWidget>
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;



struct WeatherInfo{
    QString currCity;
    QString date;
    QString week;
    QString type;
    QString curTemp;
    QString hightemp;
    QString lowtemp;
    QString fengli;
    QString fengxiang;
    QString aqi;

    QString obsTime;        //数据观测时间
    QString temp;           //温度，默认单位：摄氏度
    QString feelsLike;      //体感温度，默认单位：摄氏度
    QString text;           //天气状况的文字描述，包括阴晴雨雪等天气状态的描述
    QString windDir;        //风向
    QString windScale;      //风力等级
    QString windSpeed;      //风速，公里/小时
    QString code;           //API状态码，具体含义请参考状态码

    QString fxDate;       //预报日期
    QString textDay;      //预报白天天气状况文字描述，包括阴晴雨雪等天气状态的描述
    QString textNight;      //预报晚间天气状况文字描述，包括阴晴雨雪等天气状态的描述
    QString tempMax;      //预报当天最高温度
    QString tempMin;      //预报当天最低温度
    QString windDirDay;      //预报白天风向
    QString windScaleDay;      //预报白天风力等级



};


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void init();
    void ui_init();

    void setNetworkRequestWeather(QNetworkRequest &request, QString cityName);//设置网络请求
    void setNetworkRequestTime(QNetworkRequest &request);//设置，发送post获取网络北京时间
    void setNetworkRequestCityInfo(QString cityName);

    void getHistoryWeatherInfo(QJsonObject data);                   //获取历史天气信息
    void getTodayWeatherInfo(QJsonObject data);                     //获取当日天气信息
    void getForecastWeatherInfo(QJsonObject data);                  //获取未来天气信息
    void getOtherInfo(QJsonObject data);                            //获取其他天气信息
    void setAqi(QString &strAqi);                                   //设置空气质量指数显示
    void refreshWeather(QString str);

    void setUI_information();   //设置界面显示信息，如当前温度，空气指数等
    void splineChart(QStringList maxList, QStringList minList);//简单的线条曲线图 http://doc.qt.io/qt-5/qtcharts-splinechart-example.html
    void refresh_weather_api(QString city, QString city_id);

    QStringList read_data_file(QString file_path);
    void firest_refresh_wather();


private slots:
    void getReplyFinished(QNetworkReply *reply);

    void getReplyFinishedCityInfo(QNetworkReply *reply);

    void getReplyFinishedTime(QNetworkReply *reply);

    void on_pushButton_clicked();

    void on_reWwather_pushButton_clicked();

    void on_pushButton_setting_clicked();

    void receiveDataFromSetting(QStringList data);   //接收传递过来的数据的槽

private:
    Ui::Widget *ui;
    QNetworkAccessManager *manage_weather;
    QNetworkAccessManager *manage_cityInfo;
    QNetworkAccessManager *manage_time;


public:
    WeatherInfo todayInfo;
    WeatherInfo forecastInfo;
    //WeatherInfo todayInfo;
    QStringList provinceList;               //省份列表
    QStringList cityList;                   //城市列表
    QStringList areaList;                   //区域列表
    QStringList forecastInfo_wenduMax;  //未来天气最高温度
    QStringList forecastInfo_wenduMin;  //未来天气最低温度
    QStringList forecasetInfo_date;

    QStringList areaList_id;

};

#endif // WIDGET_H
