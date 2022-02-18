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
QNetworkRequest network_request;  //天气信息
QNetworkRequest network_request_weather_kqzl;  //天气空气质量


#define WEATHER_DAY_NUM 3
struct WeatherInfo weather_info[WEATHER_DAY_NUM];



QString global_city; //声明一个全局变量用于窗体间传值
QString global_city_id; //声明一个全局变量用于窗体间传值
QString global_data_file_path; //声明一个全局变量用于窗体间传值
//QString WeatherInfoKqzlAqi;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();

     /*设置发送数据*/
     manage_weather = new QNetworkAccessManager(this);
     manage_weather_kqzl = new QNetworkAccessManager(this);
     manage_cityInfo = new QNetworkAccessManager(this);


    /*发送get网络请求*/
     firest_refresh_wather();
     ui->currCityID_label->hide();
     ui->lineEdit_City->hide();
     ui->pushButton->hide();
     ui->showSetting_pushButton->hide();
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
    //qDebug() << __LINE__ <<  "requst_url_str------------------:"  << requst_url_str;
    request.setUrl(QUrl(QString(requst_url_str)));

    //get
    manage_city_info->get(request);
}

void Widget::setNetworkRequestWeather(QNetworkRequest &request, QString cityNameID)
{
    QString requst_url_str = QString("https://devapi.qweather.com/v7/weather/3d?location=%1&key=930cc953111c43c6924f88ebda8b00df").arg(cityNameID);
    qDebug() << __LINE__ <<  "requst_url_str---kqzl---------------:"  << requst_url_str;
    request.setUrl(QUrl(requst_url_str));
}

void Widget::setNetworkRequestWeatherKongQiZhiliang(QNetworkRequest &request, QString cityNameId)
{

    QString requst_url_str = QString("https://devapi.qweather.com/v7/air/now?location=%1&key=930cc953111c43c6924f88ebda8b00df").arg(cityNameId);
    qDebug() << __LINE__ <<  "requst_url_str------------------:"  << requst_url_str;
    request.setUrl(QUrl(requst_url_str));
}



void Widget::setNetworkRequestTime(QNetworkRequest &request)
{
    //request.setUrl(QUrl(QString("http://quan.suning.com/getSysTime.do")));
    request.setUrl(QUrl("http://quan.suning.com/getSysTime.do"));
}

void Widget::getTodayWeatherInfo(QJsonObject data)
{

//    qDebug() << __LINE__ <<  "data------------------:"  << data;

    QJsonObject daily = data.value("daily").toObject();
    //qDebug() << __LINE__ <<  "QJsonObject--daily----------------:"  << daily;

    if (data.contains(QString("daily")) && (data.value(QString("daily")).isArray()))
        {
            QJsonArray subJsonArray = data.value(QString("daily")).toArray();
            int subJsonArraySize = subJsonArray.size();
            qDebug() << __LINE__ <<  subJsonArraySize;

            for (int i = 0; i < subJsonArraySize;i++)
            {

                QJsonValue subJsonValue = subJsonArray.at(i);
                //qDebug() << __LINE__ <<  "subJsonValue------------------:"  << subJsonValue;

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


//    for(int i=0; i < 3; i++)
//    {
//        qDebug() << "weather_info[i].fxDate" <<  weather_info[i].fxDate;
//        qDebug() << "weather_info[i].textDay" <<  weather_info[i].textDay;

//    }


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

    //未来3天的，最高最低温度信息
    for(int i=0; i < WEATHER_DAY_NUM; i++)
    {

        QString wendu_max = weather_info[i].tempMax;// + QString(tr("℃"));//34℃ 最高温度
        QString wendu_min = weather_info[i].tempMin;// + QString(tr("℃"));//14℃ 最低温度
        //qDebug() << __LINE__ <<  "wendu_max: " << wendu_max << "wendu_min:" << wendu_min;
        forecastInfo_wenduMax << wendu_max;
        forecastInfo_wenduMin << wendu_min;
    }

//    qDebug() << "forecastInfo_wenduMax:" << forecastInfo_wenduMax;//forecastInfo_wenduMax: ("34℃", "33℃", "31℃", "31℃")
//    qDebug() << "forecastInfo_wenduMin:" << forecastInfo_wenduMin;

    QStringList set_chart_string_wendu_max;
    set_chart_string_wendu_max.clear();
    set_chart_string_wendu_max << forecastInfo_wenduMax ;//("Min", "", "", "0,20", "1,20", "2,20", "3,18")
//    qDebug() << "set_chart_string_wendu_max:" << set_chart_string_wendu_max;

    QStringList set_chart_string_wendu_min;
    set_chart_string_wendu_min.clear();
    set_chart_string_wendu_min << forecastInfo_wenduMin ;
//    qDebug() << "set_chart_string_wendu_min:" << set_chart_string_wendu_min;

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
    qDebug() << __FILE__ << __LINE__  << cityInfoList.removeDuplicates();
    ui->city_comboBox_c->addItems(cityInfoList);
}


void Widget::setUI_information()//设置界面显示信息，如当前温度，空气指数等
{

    for(int i=0; i < WEATHER_DAY_NUM; i++)
    {
        int weather_stu_index=0;//weather结构体的0代表是当天天气信息

        QString dangqian_min_max = tr("温度: ") + weather_info[weather_stu_index].tempMin + " ~ " + weather_info[weather_stu_index].tempMax;
        ui->dangqian_min_maxwendu_info_label->setText(dangqian_min_max);//显示当前温度范围

//        ui->dangqian_wendu_label->clear();
//        ui->dangqian_wendu_label->setText(todayInfo.temp);//显示当前实时温度

//        QString dangqian_date =  weather_info[weather_stu_index].fxDate;
//        ui->dangqian_date_info_label->setText(dangqian_date);//预报日期



        //设置天气img start
        QString textDayTmp = weather_info[weather_stu_index].textDay;
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

//绘制曲线图
void Widget::splineChart(QStringList maxList, QStringList minList)
{
    int x_jiange = 30; //x轴的间隔
    int y_jiange = 3;  //y轴的间隔

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

//      qDebug() << __LINE__ << value_x << value_y;

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
//      qDebug() << __LINE__ << value_x << value_y;
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
    //a.获取温度的最大和最小值
    int wenduzhou_max = -100;
    int wenduzhou_min = 100;
    for(int i=0; i < maxList.size(); i++)
    {
        QString tmp = maxList.at(i);
//        qDebug() << __LINE__ << tmp;
        int value = tmp.toInt();
//        qDebug() << __LINE__ << value;
        wenduzhou_max = qMax(wenduzhou_max,value);
    }

    for(int i=0; i < minList.size(); i++)
    {
        QString tmp = minList.at(i);
        int value = tmp.toInt();
        wenduzhou_min = qMin(wenduzhou_max,value);
    }
//    qDebug() << __LINE__ << "wenduzhou_max" << wenduzhou_max << "wenduzhou_min" << wenduzhou_min;

    axisY->setRange(wenduzhou_min - y_jiange , wenduzhou_max + y_jiange);
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

//connect(manage_weather,SIGNAL(finished(QNetworkReply *)),this,SLOT(getReplyFinished(QNetworkReply*)));//, Qt::UniqueConnection);
/*
 * Qt::UniqueConnection是为了防止一次信号，多次槽函数。connect建立部分，放到初始化部分，只实例化时调用一次，避免了多次创建connect信号与槽的问题。
其实问题的根本原因：就是要避免多次创建信号与槽。 多了解一下信号与槽的建立机制，或者在使用的时候，加上限制参数
*/


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
        connect(manage_weather,SIGNAL(finished(QNetworkReply*)),this,SLOT(getReplyFinished(QNetworkReply*)), Qt::UniqueConnection);
        manage_weather->get(network_request);
    }
}

void Widget::refresh_weather_kqzl_api(QString city, QString city_id)
{
    qDebug() << __LINE__ << "city_id:" << city_id;
    qDebug() << __LINE__ << "city:" << city;

    if(!city_id.isEmpty())
    {
        //发送天气请求，获取的数据getReplyFinished进行处理
        setNetworkRequestWeatherKongQiZhiliang(network_request_weather_kqzl, city_id);
        connect(manage_weather_kqzl,SIGNAL(finished(QNetworkReply*)),this,SLOT(getReplyFinishedForWeatherKqzl(QNetworkReply*)), Qt::UniqueConnection);
        manage_weather_kqzl->get(network_request_weather_kqzl);
    }
}

QStringList Widget::read_data_file(QString file_path)
{
    QString AppDirPath  = QApplication::applicationDirPath();
    qDebug() << "AppDirPath:" << AppDirPath;
    QString fileName = AppDirPath + "/" + "data.txt";
    qDebug() << "fileName:" << fileName;
//    global_data_file_path = fileName;

    QFile f(fileName);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()  << "Open failed.";
        QMessageBox::warning(this,"file error","找不到配置文件",QMessageBox::Yes);
    }


    QTextStream txtOutput(&f);
    QString lineStr;
    QStringList all_line_str;
    while(!txtOutput.atEnd())
    {
        lineStr = txtOutput.readLine().trimmed();
        all_line_str.append(lineStr);
    }
    qDebug()  << __LINE__ << "@@@@@@@all_lineStr:" << all_line_str;

    qDebug()  << file_path;
    f.close();
   // f.remove(file_path);
    return all_line_str;
}

void Widget::firest_refresh_wather()
{
    QStringList city_info = read_data_file(global_data_file_path);
    if(city_info.size() == 2)
    {
        refresh_weather_kqzl_api(city_info.first(), city_info.last());
        refresh_weather_api(city_info.first(), city_info.last());
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

//获取处理空气质量后进行解析
void Widget::getReplyFinishedForWeatherKqzl(QNetworkReply *reply)
{
    QJsonObject json_data = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << __LINE__ << "Json 空气质量:" << json_data;

    QJsonObject now = json_data.value("now").toObject();
    qDebug() << __LINE__ <<  "QJsonObject--now----------------:"  << now;
    QString now_api = now.value("aqi").toString();

    qDebug() << __LINE__ <<  "now_api----------------:"  << now_api;


    ui->dangqian_kongqizhiliang_info_label->setText(tr("实时空气质量:"));
    ui->dangqian_kongqizhiliang_img_label->setText(now_api);//显示当前空气质量
    int aqi = now_api.toInt();

    //qDebug() << "aqi:" << aqi;
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

}

void Widget::getReplyFinishedCityInfo(QNetworkReply *reply)
{
    QJsonObject json_citydata = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << __LINE__ << "Json 城市信息:" << json_citydata;

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




//显示City选项
void Widget::on_showSetting_pushButton_clicked()
{

    //ui->showSetting_pushButton->hide();

    ui->city_comboBox_p->show();
    ui->city_comboBox_c->show();
    ui->city_comboBox_a->show();

    ui->lineEdit_City->show();
    ui->lineEdit_City->setPlaceholderText(tr("请输入要查询的城市？"));

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
       // refreshWeather(city_id);
       // refreshWeatherKongqizhiliang(city_id);
    }
}


void Widget::on_pushButton_clicked()
{
    qDebug()<<"QSslSocket="<<QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();
#if 1
    manage_time = new QNetworkAccessManager(this);
    connect(manage_time, SIGNAL(finished(QNetworkReply*)), this, SLOT(getReplyFinishedTime(QNetworkReply*)));

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
        firest_refresh_wather();
    }

}
