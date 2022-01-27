#include "widget.h"
#include "ui_widget.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "settingform.h"

QNetworkRequest network_request_cityinfo;
QNetworkRequest network_request_time;
QNetworkRequest network_request;

#define WEATHER_DAY_NUM 3
struct WeatherInfo weather_info[WEATHER_DAY_NUM];



QString global_city; //声明一个全局变量用于窗体间传值
QString global_city_id; //声明一个全局变量用于窗体间传值



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();

     /*设置发送数据*/
     manage_weather = new QNetworkAccessManager(this);
     manage_cityInfo = new QNetworkAccessManager(this);
//     QString cityName = "北京";
//     QString provName = "北京";
//     qDebug() << __LINE__ << "init cityName:" << cityName;

     //发送请求
     //setNetworkRequestWeather(network_request, cityName);
    // connect(manage,SIGNAL(finished(QNetworkReply *)),this,SLOT(getReplyFinished(QNetworkReply*)));

     //setNetworkRequestCityInfo(network_request_cityinfo,provName);
     //connect(manage_cityInfo, SIGNAL(finished(QNetworkReply *)), this, SLOT(getReplyFinishedCityInfo(QNetworkReply*)));




    /*发送get网络请求*/
    //manage->get(network_request);

   // manage_cityInfo->get(network_request_cityinfo);
//    manage_time->get(network_request_time);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    ui_init();
    setWindowTitle(tr("天气预报"));
}

void Widget::ui_init()//界面初始化
{
    ui->dangqian_wendu_label->clear();

    ui->city_comboBox_p->hide();
    ui->city_comboBox_c->hide();
    ui->city_comboBox_a->hide();

    ui->save_pushButton->hide();
    getProvinceList();
}

//搜索城市ID
//查询地区的名称，支持文字、以英文逗号分隔的经度,纬度坐标（十进制，最多支持小数点后两位）、LocationID或Adcode（仅限中国城市）。例如 location=北京 或 location=116.41,39.92
void Widget::setNetworkRequestCityInfo(QString cityName)
{
    QNetworkRequest request;
    QNetworkAccessManager *manage_city_info;
    manage_city_info = new QNetworkAccessManager(this);
    connect(manage_city_info, SIGNAL(finished(QNetworkReply *)), this, SLOT(getReplyFinishedCityInfo(QNetworkReply*)));

    QString requst_url_str = QString("https://geoapi.qweather.com/v2/city/lookup?location=%1&key=930cc953111c43c6924f88ebda8b00df").arg(cityName);
    qDebug() << __LINE__ <<  "requst_url_str------------------:"  << requst_url_str;

    request.setUrl(QUrl(QString(requst_url_str)));

    //get
    manage_city_info->get(request);
}

void Widget::setNetworkRequestWeather(QNetworkRequest &request, QString cityNameID)
{
    QString requst_url_str = QString("https://devapi.qweather.com/v7/weather/3d?location=%1&key=930cc953111c43c6924f88ebda8b00df").arg(cityNameID);
    qDebug() << __LINE__ <<  "requst_url_str------------------:"  << requst_url_str;

    request.setUrl(QUrl(requst_url_str));
}

//void Widget::setNetworkRequestCityInfo(QNetworkRequest &request, QString Name)
//{

//    request.setUrl(QUrl(QString("https://geoapi.qweather.com/v2/city/lookup?location=%1&key=930cc953111c43c6924f88ebda8b00df")
//                                .arg(Name)));


//}

void Widget::setNetworkRequestTime(QNetworkRequest &request)
{
    //request.setUrl(QUrl(QString("http://quan.suning.com/getSysTime.do")));
    request.setUrl(QUrl("http://quan.suning.com/getSysTime.do"));

   // request.setUrl(QUrl("https://devapi.qweather.com/v7/weather/now?location=101010100&key=930cc953111c43c6924f88ebda8b00df"));



}

void Widget::getTodayWeatherInfo(QJsonObject data)
{

//    qDebug() << __LINE__ <<  "data------------------:"  << data;

    QJsonObject daily = data.value("daily").toObject();
    qDebug() << __LINE__ <<  "QJsonObject--daily----------------:"  << daily;

    if (data.contains(QString("daily")) && (data.value(QString("daily")).isArray()))
        {
            QJsonArray subJsonArray = data.value(QString("daily")).toArray();
            int subJsonArraySize = subJsonArray.size();
            qDebug() << __LINE__ <<  subJsonArraySize;

            for (int i = 0; i < subJsonArraySize;i++)
            {

                QJsonValue subJsonValue = subJsonArray.at(i);
                qDebug() << __LINE__ <<  "subJsonValue------------------:"  << subJsonValue;

                if (subJsonValue.isObject())
                {
                    QJsonObject subJsonObject = subJsonValue.toObject();

                    //预报日期
                    QString fxDateTmp = subJsonObject.value("fxDate").toString();
                    if(fxDateTmp.isEmpty())
                    {
                       weather_info[i].fxDate = " ";
                    }
                    else
                    {
                       weather_info[i].fxDate = fxDateTmp;
                    }

                    //预报白天天气状况文字描述，包括阴晴雨雪等天气状态的描述
                    QString textDayTmp = subJsonObject.value("textDay").toString();
                    if(textDayTmp.isEmpty())
                    {
                       weather_info[i].textDay = " ";
                    }
                    else
                    {
                       weather_info[i].textDay = textDayTmp;
                    }

                    //预报当天最高温度
                    QString tempMaxTmp = subJsonObject.value("tempMax").toString();
                    if(tempMaxTmp.isEmpty())
                    {
                       weather_info[i].tempMax = " ";
                    }
                    else
                    {
                       weather_info[i].tempMax = tempMaxTmp;
                    }

                    //预报当天最低温度
                    QString tempMinTmp = subJsonObject.value("tempMin").toString();
                    if(tempMinTmp.isEmpty())
                    {
                       weather_info[i].tempMin = " ";
                    }
                    else
                    {
                       weather_info[i].tempMin = tempMinTmp;
                    }

                    //预报白天风向
                    QString windDirDayTmp = subJsonObject.value("windDirDay").toString();
                    if(windDirDayTmp.isEmpty())
                    {
                       weather_info[i].windDirDay = " ";
                    }
                    else
                    {
                       weather_info[i].windDirDay = windDirDayTmp;
                    }

                    //预报白天风力等级
                    QString windScaleDayTmp = subJsonObject.value("windScaleDay").toString();
                    if(windScaleDayTmp.isEmpty())
                    {
                       weather_info[i].windScaleDay = " ";
                    }
                    else
                    {
                       weather_info[i].windScaleDay = windScaleDayTmp;
                    }




#if 0
                    //使用QVariant的方式获取value对应的值
                    if (subJsonObject.contains(QString("fxDate")) && (QVariant(subJsonObject.value(QString("fxDate"))).canConvert<QString>()))
                    {
                        QString fxDateTmp = QString(QVariant(subJsonObject.value(QString("fxDate"))).value<QString>());
                        if(fxDateTmp.isEmpty())
                        {
                           weather_info[i].fxDate = " ";
                        }
                        else
                        {
                           weather_info[i].fxDate = fxDateTmp;
                        }
                    }
                    if (subJsonObject.contains(QString("textDay")) && (QVariant(subJsonObject.value(QString("textDay"))).canConvert<QString>()))
                    {
                        QString textDayTmp = QString(QVariant(subJsonObject.value(QString("textDay"))).value<QString>());
                        if(textDayTmp.isEmpty())
                        {
                           weather_info[i].textDay = " ";
                        }
                        else
                        {
                           weather_info[i].textDay = textDayTmp;
                        }
                    }
#endif
                }
            }
        }


    for(int i=0; i < 3; i++)
    {
        qDebug() << "weather_info[i].fxDate" <<  weather_info[i].fxDate;
        qDebug() << "weather_info[i].textDay" <<  weather_info[i].textDay;

    }




#if 0

    todayInfo.currCity = data.value("retData").toObject().value("city").toString();
    qDebug() << "currCity:" << todayInfo.currCity;
    todayInfo.date = today.value("date").toString();//2016-08-26
    todayInfo.week = today.value("week").toString();//星期五
    todayInfo.type = today.value("type").toString();//阵雨
    todayInfo.curTemp = today.value("curTemp").toString();//30℃
    todayInfo.hightemp = today.value("hightemp").toString();//34℃
    todayInfo.lowtemp = today.value("lowtemp").toString();//24℃
    todayInfo.fengli = today.value("fengli").toString();//微风级
    todayInfo.fengxiang = today.value("fengxiang").toString();//东北风
    todayInfo.aqi = today.value("aqi").toString();//60

    qDebug() << "getTodayWeatherInfo:\n" << todayInfo.currCity + todayInfo.date + todayInfo.week << todayInfo.type << todayInfo.curTemp
             << todayInfo.hightemp << todayInfo.lowtemp << todayInfo.fengli << todayInfo.fengxiang
             << todayInfo.aqi;
#endif


    setUI_information();//设置UI上的信息



}

//处理未来天气，并使用splineChart,画出曲线图
void Widget::getForecastWeatherInfo(QJsonObject data)
{
    forecastInfo_wenduMax.clear();
    forecastInfo_wenduMin.clear();
    /*
    QStringList forecastInfoList;
    QJsonArray forecastArray = data.value("retData").toObject().value("forecast").toArray();
    int size = forecastArray.size();
    qDebug() << "size;" << size;

    for(int i=0; i < size; i++)
    {
        QJsonObject forecast = forecastArray.at(i).toObject();

        forecastInfo.date = forecast.value("date").toString();
        forecastInfo.week = forecast.value("week").toString();
        forecastInfo.type = forecast.value("type").toString();
        forecastInfo.curTemp = forecast.value("curTemp").toString();
        forecastInfo.hightemp = forecast.value("hightemp").toString();
        forecastInfo.lowtemp = forecast.value("lowtemp").toString();
        forecastInfo.fengli = forecast.value("fengli").toString();
        forecastInfo.fengxiang = forecast.value("fengxiang").toString();
        forecastInfo.aqi = forecast.value("aqi").toString();
//        qDebug() << forecastInfo.date + forecastInfo.week << forecastInfo.type << forecastInfo.curTemp
//                 << forecastInfo.hightemp << forecastInfo.lowtemp << forecastInfo.fengli << forecastInfo.fengxiang
//                 << forecastInfo.aqi;

        QString foreInfo = forecastInfo.date + "," + forecastInfo.week + "," +  forecastInfo.type + "," + forecastInfo.type + "," +
                           forecastInfo.hightemp + "," + forecastInfo.lowtemp + "," + forecastInfo.fengli + "," + forecastInfo.fengxiang +"," + forecastInfo.aqi;
        qDebug() << "foreInfo:" << foreInfo;//"2016-09-03,星期六,多云,多云,34℃,25℃,微风级,东风,"
        forecastInfoList << foreInfo;
        forecasetInfo_date << forecastInfo.date;

        qDebug() << "forecastInfoList:" << forecastInfoList;

    }


    for(int i=0, j=1; i < forecastInfoList.size(); i++)
    {
        QString tmp = forecastInfoList.at(i);
        qDebug() << tmp;
       // QString dd = tmp.section(',', 4, 4);//34℃
       // QString ddd = dd.mid(0,2);//mid()函数接受两个参数，第一个是起始位置，第二个是取串的长度。34℃ -> 34
        //QString i_str = QString::number(dd,10);
       // qDebug() << "ddd: " << ddd;
        QString wendu_max =QString::number(j,10) + "," + tmp.section(',', 4, 4).mid(0,2);//34℃ 最高温度
        QString wendu_min =QString::number(j,10) + "," + tmp.section(',', 5, 5).mid(0,2);//14℃ 最低温度
        j = j + 2;
        qDebug() << "wendu_max: " << wendu_max << "wendu_min:" << wendu_min;
       // wendu_max =  i + "," + wendu_max;
        forecastInfo_wenduMax << wendu_max;
        forecastInfo_wenduMin << wendu_min;
    }
*/
    //未来3天的，最高最低温度信息
    for(int i=0; i < WEATHER_DAY_NUM; i++)
    {

        QString wendu_max = weather_info[i].tempMax;// + QString(tr("℃"));//34℃ 最高温度
        QString wendu_min = weather_info[i].tempMin;// + QString(tr("℃"));//14℃ 最低温度
        qDebug() << __LINE__ <<  "wendu_max: " << wendu_max << "wendu_min:" << wendu_min;
        forecastInfo_wenduMax << wendu_max;
        forecastInfo_wenduMin << wendu_min;
    }


    qDebug() << "forecastInfo_wenduMax:" << forecastInfo_wenduMax;//forecastInfo_wenduMax: ("34℃", "33℃", "31℃", "31℃")
    qDebug() << "forecastInfo_wenduMin:" << forecastInfo_wenduMin;

    QStringList set_chart_string_wendu_max;
    set_chart_string_wendu_max.clear();
    set_chart_string_wendu_max << forecastInfo_wenduMax ;//("Min", "", "", "0,20", "1,20", "2,20", "3,18")
    qDebug() << "set_chart_string_wendu_max:" << set_chart_string_wendu_max;

    QStringList set_chart_string_wendu_min;
    set_chart_string_wendu_min.clear();
    set_chart_string_wendu_min << forecastInfo_wenduMin ;
    qDebug() << "set_chart_string_wendu_min:" << set_chart_string_wendu_min;

    splineChart(set_chart_string_wendu_max, set_chart_string_wendu_min);



}




void Widget::getOtherInfo(QJsonObject data)
{
    QJsonArray otherArray = data.value("retData").toObject().value("today").toObject().value("index").toArray();

    QString index, details,name;
    QJsonObject otherInfo = otherArray.at(0).toObject();//感冒指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(1).toObject();//防晒指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details << "name:" <<name;
    ui->fangshaizhishu_label_1->setText(name);
    ui->fangshaizhishu_label_2->setText(index);

    otherInfo = otherArray.at(2).toObject();//穿衣指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(3).toObject();//运动指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;
    ui->yongdongzhishu_label_5->setText(name);
    ui->yongdongzhishu_label_6->setText(index);

    otherInfo = otherArray.at(4).toObject();//洗车指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;
    ui->xichezhishu_label_7->setText(name);
    ui->xichezhishu_label_8->setText(index);

    otherInfo = otherArray.at(5).toObject();//晾晒指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;
    ui->liangshaizhishu_label_3->setText(name);
    ui->liangshaizhishu_label_4->setText(index);

}

//刷新天气。传入参数为:城市ID
void Widget::refreshWeather(QString str)
{
    qDebug() << __LINE__ <<  "city_id" << str;
    setNetworkRequestWeather(network_request, str);
    connect(manage_weather,SIGNAL(finished(QNetworkReply *)),this,SLOT(getReplyFinished(QNetworkReply*)));
    manage_weather->get(network_request);
}

//void Widget::refreshCityInfo(QString str)
//{

////    setNetworkRequestCityInfo(network_request_cityinfo,str);
////    manage_cityInfo->get(network_request_cityinfo);
//}


void Widget::getProvinceList()
{
    QStringList proName;
    proName << "北京" << "上海"<< "天津" << "重庆" << "黑龙江" << "吉林" << "辽宁" << "内蒙古" << "河北" << "山西" << "陕西" << "山东" << "新疆" << "西藏" << "青海" << "甘肃" << "宁夏" << "河南" << "江苏" << "湖北" << "浙江" << "安徽" << "福建" << "江西" << "湖南" << "贵州" << "四川" << "广东" << "云南" << "广西" << "海南" << "香港" << "澳门" << "台湾";
    ui->city_comboBox_p->clear();
    ui->city_comboBox_p->addItems(proName);
}

void Widget::getCityList(QJsonObject data)
{
     QStringList cityInfoList;
     cityInfoList.clear();
     QJsonArray cityinfo = data.value("retData").toArray();
     int size = cityinfo.size();
     qDebug() << "cityinfo.......................size;" << size;
     for(int i=0; i < size; i++)
     {
         QJsonObject tmp = cityinfo.at(i).toObject();
         QString province_cn = tmp.value("province_cn").toString();
         if(province_cn == ui->city_comboBox_p->currentText())
         {
             QString district_cn = tmp.value("district_cn").toString();
             cityInfoList << district_cn;
         }

     }
    qDebug() << "777777777777777777" << cityInfoList.removeDuplicates();
    ui->city_comboBox_c->addItems(cityInfoList);

}


void Widget::getAreaList(QJsonObject data)
{
    areaList.clear();
    areaList_id.clear();

    ui->city_comboBox_a->clear();
    QJsonArray cityinfo = data.value("location").toArray();
    int size = cityinfo.size();
    qDebug() << "cityinfo.......................size;" << size;
    for(int i=0; i < size; i++)
    {
        QJsonObject tmp = cityinfo.at(i).toObject();
        QString district_cn = tmp.value("name").toString();
        qDebug() << __LINE__ << "district_cn" << district_cn;
        areaList << district_cn;

        QString city_id_tmp = tmp.value("id").toString();
        qDebug() << __LINE__ << "city_id_tmp" << city_id_tmp;
        areaList_id << city_id_tmp;


    }

    qDebug() << __LINE__ << "areaList" << areaList;
    qDebug() << __LINE__ << "areaList_id" << areaList_id;

    if(areaList.size() > 0)
    {
        QString city = areaList.first();

        if(city.isEmpty())
        {
           ui->currCity_label->clear();
        }
        else
        {
           ui->currCity_label->setText(city);

        }
    }

    if(areaList_id.size() > 0)
    {
        QString city_id = areaList_id.first();

        if(city_id.isEmpty())
        {
           ui->currCityID_label->clear();
        }
        else
        {
           ui->currCityID_label->setText(city_id);

        }
    }


    ui->city_comboBox_a->addItems(areaList);


}




void Widget::setUI_information()//设置界面显示信息，如当前温度，空气指数等
{

    for(int i=0; i < WEATHER_DAY_NUM; i++)
    {
        int demo_int=0;
//        ui->currCity_label->clear();
//        ui->currCity_label->setText(todayInfo.code);//显示当前城市

        QString dangqian_min_max = tr("温度: ") + weather_info[demo_int].tempMin + " ~ " + weather_info[demo_int].tempMax;
        ui->dangqian_min_maxwendu_info_label->setText(dangqian_min_max);//显示当前温度范围

//        ui->dangqian_wendu_label->clear();
//        ui->dangqian_wendu_label->setText(todayInfo.temp);//显示当前实时温度

//        QString dangqian_date =  weather_info[demo_int].fxDate;
//        ui->dangqian_date_info_label->setText(dangqian_date);//预报日期

        ui->dangqian_kongqizhiliang_info_label->setText(tr("实时空气质量:"));
        ui->dangqian_kongqizhiliang_img_label->setText("40");//显示当前空气质量
        int aqi = 40;
        qDebug() << "aqi:" << aqi;
        if(aqi >=0 && aqi <= 50 )
        {
          ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#51FF00;");
        }
        if(aqi > 50 && aqi <= 100 )
        {
          ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#AFDB00;");
        }
        if(aqi > 100 && aqi <= 200 )
        {
          ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#FF6666;");
        }
        if(aqi > 200)
        {
          ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#CC0033;");
        }


        //设置天气img start
        QString textDayTmp = weather_info[demo_int].textDay;
        if(textDayTmp == "晴")
        {
            QPixmap tianqi_pixmap(":/img/ico/qing.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp == "阴")
        {
            QPixmap tianqi_pixmap(":/img/ico/yin.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp == "阵雨" || textDayTmp == "雷阵雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/leizhenyu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp == "多云")
        {
            QPixmap tianqi_pixmap(":/img/ico/duoyun.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp == "小雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/xiaoyu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp == "中雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/zhongyu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp == "大雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/dayu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp.contains("雪"))
        {
            QPixmap tianqi_pixmap(":/img/ico/xue.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else if(textDayTmp.contains("雨夹雪"))
        {
            QPixmap tianqi_pixmap(":/img/ico/yujiaxue.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        else
        {
            QPixmap tianqi_pixmap(":/img/ico/undefined.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        //设置天气img end

    }


}

void Widget::splineChart(QStringList maxList, QStringList minList)
{
    int x_jiange = 30;
    int y_jiange = 10;

    qDebug() << __LINE__ << "maxList:" << maxList << maxList.size();
    qDebug() << __LINE__ << "minList:" << minList << minList.size();

    if(maxList.size() < 3 || minList.size() < 3)
    {
        qDebug() << __LINE__ << "maxList  minList error. return";
        return;
    }


    QSplineSeries *seriesMax = new QSplineSeries();//曲线 //new QLineSeries();//折线
    QSplineSeries *seriesMin = new QSplineSeries();//曲线

    seriesMax->setName("Max");//设置曲线Max的标题内容
    seriesMin->setName("Min");//设置曲线Min的标题内容
    seriesMax->setPen(QPen(Qt::red,2,Qt::SolidLine));//设置曲线颜色宽度
    seriesMin->setPen(QPen(Qt::blue,2,Qt::SolidLine));



    int max_length = maxList.size();
    int max_tmp = max_length - 1; //画曲线点的最后一点
    for(int i=0; i < maxList.size(); i++)//Max曲线上添加点坐标
    {
      QString tmp = maxList.at(i);
      int value_x = x_jiange*(i+1);
      int value_y = tmp.toInt();
      if(i == 0)
      {
            value_x = 15*(i+1);
      }
      if(i == max_tmp)
      {
            value_x = value_x - 15;
      }

      qDebug() << __LINE__ << value_x << value_y;

      seriesMax->append(value_x,value_y);
    }

    for(int i=0; i < minList.size(); i++)//Min曲线上添加点坐标
    {
      QString tmp = minList.at(i);
      int value_x = x_jiange*(i+1);
      int value_y = tmp.toInt();
      if(i == 0)
      {
            value_x = 15*(i+1);
      }
      if(i == max_tmp)
      {
            value_x = value_x - 15;
      }
      qDebug() << __LINE__ << value_x << value_y;
      seriesMin->append(value_x,value_y);
    }

//    qDebug() << "x_max: " << x_max << ",x_min" << x_min;//x轴上对应的点的最大值和最小值
//    qDebug() << "y_max: " << y_max << ",y_min" << y_min;//y轴上对应的点的最大值和最小值



    QCategoryAxis *axisX = new QCategoryAxis();
    QValueAxis    *axisY = new QValueAxis;      //懒人版    //http://blog.csdn.net/linbounconstraint/article/details/52440807

    //自定义XY轴上显示的label的颜色 Customize axis label colors
    //QBrush axisBrush(Qt::black);
    //axisX->setLabelsBrush(axisBrush);
    //axisY->setLabelsBrush(axisBrush);
    //axisX->setLabelFormat("%g"); //设置刻度的格式
    //axisX->setTitleText(""); //设置X轴的标题
    //axisX->setGridLineVisible(true); //设置是否显示网格线
    //axisX->setMinorTickCount(4); //设置小刻度线的数目
    //axisX->setLabelsVisible(false); //设置刻度是否显示

    /*
        设置X轴上label的显示内容和范围
        ("a", 2), 	("b", 4), 	("c", 6),	("d", 8),
        Pairt(1,20) Pairt(3,20) Pairt(5,20) Pairt(7,20)
        //    axisX->append("a", 1);
    */
//    if(!forecasetInfo_date.isEmpty())
//    {
//        for(int j = 1; j <= forecasetInfo_date.size(); j++)
//        {
//            axisX->append(forecasetInfo_date.at(j-1), j*2);//axisX->append("a", 1);
//        }
//        qDebug() << "[leo]forecasetInfo_date.size():" << forecasetInfo_date.size();
//        axisX->setRange(0,  forecasetInfo_date.size()*2);
//    }
//    forecasetInfo_date.clear();


    //日期轴
    axisX->append("今天",30);
    axisX->append("明天",60);
    axisX->append("后天",90);
    axisX->setRange(0,  90);

    //温度轴
    axisY->setRange(-10 , 15);
    axisY->setLabelFormat("%d");
    //axisY->setGridLineVisible(true);//设置刻度是否显示


    QChart *chart = new QChart();

    chart->addSeries(seriesMax);//把曲线Max添加到chart上
    chart->addSeries(seriesMin);//把曲线Min添加到chart上
    chart->setTitle(tr("未来三天天温度走势图"));
    chart->setAnimationOptions(QChart::GridAxisAnimations);//设置曲线呈动画显示
    //chart->createDefaultAxes();//创建曲线的轴 默认值
    chart->setAxisX(axisX, seriesMax);//把曲线Max加载到日期轴
    chart->setAxisY(axisY, seriesMax);//把曲线Max加载到温度轴

    chart->setAxisX(axisX, seriesMin);//把曲线Min加载到日期轴
    chart->setAxisY(axisY, seriesMin);//把曲线Min加载到温度轴


//    if(maxList.at(0) != "")//valueList的第一个字符内容，如果内容为空则隐藏legend，否则显示字符内容为标题
//    {
//      chart->legend()->show();
//    }
//    else
//    {
//      chart->legend()->hide();
//    }


//    QString axisX_str = maxList.at(1);//设置X轴的范围
//    if(axisX_str != "")
//    {
//      qDebug() << axisX_str;
//      int axisX_p = axisX_str.section(',', 0, 0).toInt();//"5,8" int:  5
//      int axisX_l = axisX_str.section(',', 1, 1).toInt();//"5,8" int:  8
//      chart->axisX()->setRange(axisX_p, axisX_l);//设置X轴的范围，如果不设置，将默认取当前线段上的点的最大x,y的值作为最大range
//    }

//    QString axisY_str = maxList.at(2);//设置Y轴的范围
//    if(axisY_str != "")
//    {
//      qDebug() << axisY_str;
//      int axisY_p = axisY_str.section(',', 0, 0).toInt();//"5,8" int:  5
//      int axisY_l = axisY_str.section(',', 1, 1).toInt();//"5,8" int:  8
//      chart->axisY()->setRange(axisY_p, axisY_l);//设置Y轴的范围，如果不设置，将默认取当前线段上的点的最大x,y的值作为最大range
//    }


    ui->chart_widget->setRenderHint(QPainter::Antialiasing);//防止曲线出现“锯齿”现象
    ui->chart_widget->setChart(chart);

}


void Widget::refresh_weather_api(QString city, QString city_id)
{
    qDebug() << __LINE__ << "city_id:" << city_id;
    qDebug() << __LINE__ << "city:" << city;

    if(!city_id.isEmpty())
    {
        //刷新天气
        ui->currCity_label->setText(city);
        ui->currCityID_label->setText(city_id);
        //发送天气请求，获取的数据getReplyFinished进行处理
        setNetworkRequestWeather(network_request, city_id);
        connect(manage_weather,SIGNAL(finished(QNetworkReply *)),this,SLOT(getReplyFinished(QNetworkReply*)));
        manage_weather->get(network_request);
    }
}


//获取天气信息后进行解析
void Widget::getReplyFinished(QNetworkReply *reply)//获取天气api传回的数据
{
    QJsonObject json_data = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << __LINE__ << "Json 天气信息:" << json_data;

    //获取历史天气信息
    //getHistoryWeatherInfo(json_data);

    //获取当日天气信息
     getTodayWeatherInfo(json_data);

    //获取未来天气信息
     getForecastWeatherInfo(json_data);

    //获取其他天气信息
    //getOtherInfo(json_data);


}

void Widget::getReplyFinishedCityInfo(QNetworkReply *reply)
{
    QJsonObject json_citydata = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << __LINE__ << "Json 城市信息:" << json_citydata;


    //getProvinceList();
    //getCityList(json_citydata);
    getAreaList(json_citydata);

}

void Widget::getReplyFinishedTime(QNetworkReply *reply)
{

    if (reply->error() == QNetworkReply::NoError)
    {
         QByteArray bytes = reply->readAll();
         qDebug() << __LINE__ << bytes;
         QJsonObject json_timedata = QJsonDocument::fromJson(reply->readAll()).object();
         qDebug() <<"object size:"<< json_timedata.size();
//         qDebug() << "Json 时间信息:" << json_timedata;
//         QJsonArray Array=json_timedata["sysTime2"].toArray();
//         qDebug() << "Json Array:" << Array;


//         for (QJsonObject::Iterator it = json_timedata.begin();it != json_timedata.end(); it++)
//         {
//            QJsonValue value=it.value();
//            QJsonArray Array=value.toArray();//将value转成QJsonArray或QJsonObject，继续遍历
//            qDebug()<< value.toString();

//         }

         if(reply->url()==QUrl("http://quan.suning.com/getSysTime.do"))
         {
             qDebug() << __LINE__ << bytes;
         }
         else
         {
             qDebug() << __LINE__ << bytes;
         }

     }
     else
     {
         qDebug() << "finishedSlot errors here";
         qDebug( "found error .... code: %d\n", (int)reply->error());
     }

     reply->deleteLater();
}



//void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
//{
//    qDebug() << __LINE__ <<  arg1;
//    //refreshWeather(arg1);
//}


//显示City选项
void Widget::on_showSetting_pushButton_clicked()
{

    ui->save_pushButton->show();
    //ui->showSetting_pushButton->hide();

    ui->city_comboBox_p->show();
    ui->city_comboBox_c->show();
    ui->city_comboBox_a->show();


    ui->lineEdit_City->show();
    ui->lineEdit_City->setPlaceholderText(tr("请输入要查询的城市？"));

}


void Widget::on_save_pushButton_clicked()
{

   qDebug() << __FILE__ << __LINE__ << "global_city" << global_city;
   qDebug() << __FILE__ << __LINE__ << "global_city_id" << global_city_id;

   if(global_city.isEmpty())
   {
      ui->currCity_label->clear();
   }
   else
   {
      ui->currCity_label->setText(global_city);
   }

   if(global_city_id.isEmpty())
   {
      ui->currCityID_label->clear();
   }
   else
   {
      ui->currCityID_label->setText(global_city_id);
   }

  //  return;
//    ui->save_pushButton->hide();
//    ui->city_comboBox_p->hide();
//    ui->city_comboBox_c->hide();
//    ui->city_comboBox_a->hide();

    ui->showSetting_pushButton->show();
    //ui->currCity_label->setText(todayInfo.currCity);


//    QString city = ui->lineEdit_City->text().trimmed();
//    qDebug() << __LINE__ << "city:" << city;
//    setNetworkRequestCityInfo(city);
    //ui->lineEdit_City->hide();

//    qDebug() << __LINE__ << "areaList:" << areaList;
//    qDebug() << __LINE__ << "areaList_id:" << areaList_id;

//    city = areaList.first();
//    qDebug() << __LINE__ << "city:" << city;

    //
    //QString city_id = areaList_id.first();
    //qDebug() << __LINE__ << "city_id:" << city_id;


//    if(!city_id.isEmpty())
//    {
//        ui->currCityID_label->setText(city_id);
//        ui->currCity_label->setText(city);
//        refreshWeather(city_id);
//    }


}
void Widget::on_reWwather_pushButton_clicked()
{
    QString city_id = global_city_id;
    QString city = global_city;

    qDebug() << __LINE__ << "city_id:" << city_id;
    qDebug() << __LINE__ << "city:" << city;

    if(!city_id.isEmpty())
    {
        //刷新天气
        ui->currCity_label->setText(city);
        ui->currCityID_label->setText(global_city_id);
        refreshWeather(city_id);
    }
}


//void Widget::on_city_comboBox_p_activated(const QString &arg1)
//{
//    ui->city_comboBox_c->clear();
//    refreshCityInfo(arg1);
//}

//void Widget::on_city_comboBox_c_activated(const QString &arg1)
//{
//     refreshCityInfo(arg1);
//}

void Widget::on_pushButton_clicked()
{
    qDebug()<<"QSslSocket="<<QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();
#if 1
    manage_time = new QNetworkAccessManager(this);
    connect(manage_time, SIGNAL(finished(QNetworkReply *)), this, SLOT(getReplyFinishedTime(QNetworkReply*)));

    setNetworkRequestTime(network_request_time);

    //get
    manage_time->get(network_request_time);


#else

    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);

    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getReplyFinishedTime(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl("http://quan.suning.com/getSysTime.do"));

    //get
    accessManager->get(request);
#endif
}

//保存天气的城市，城市ID等设置
void Widget::on_pushButton_setting_clicked()
{
    SettingForm *setting_widget = new SettingForm();
    //关联信号和槽函数
    connect(setting_widget, SIGNAL(sendDataFromSettingToWidget(QStringList)), this, SLOT(receiveDataFromSetting(QStringList)));

    setting_widget->setWindowTitle("设置");
    setting_widget->show();
}

void Widget::receiveDataFromSetting(QStringList data)
{
    qDebug() << __LINE__ << "传递过来的数据="<< data;
    if(data.size() == 2)
    {
        qDebug() << __FILE__ << __LINE__ << "传递过来的数据="<< data.first();
        qDebug() << __FILE__ << __LINE__ << "传递过来的数据="<< data.last();
        refresh_weather_api(data.first(), data.last());
    }


}
