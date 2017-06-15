

#include <iostream>
#include <fstream>				  
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "slqAlexNet.h"

using namespace std;
using namespace cv;

namespace slqDL {
    namespace slqAlexNet {

        slqAlexNet::~slqAlexNet()
        {
            if (inRaw)
            {
                delete inRaw;
                inRaw = nullptr;
            }
            deletevar(&mlabel);
            deletevar(&inMap);
            deletevar(&c1Map);
            deletevar(&s1Map);
            deletevar(&c2Map);
            deletevar(&s2Map);
            deletevar(&c3Map);
            deletevar(&c4Map);
            deletevar(&c5Map);
            deletevar(&s5Map);
            deletevar(&f1Map);
            deletevar(&f2Map);
            deletevar(&f3Map);

            deletevar(&c1Conv);
            deletevar(&s1Pool);
            deletevar(&c2Conv);
            deletevar(&s2Pool);
            deletevar(&c3Conv);
            deletevar(&c4Conv);
            deletevar(&c5Conv);
            deletevar(&s5Pool);
            deletevar(&f1Conn);
            deletevar(&f2Conn);
            deletevar(&f3Conn);

            deletevar(&c1Bias);
            deletevar(&s1Bias);
            deletevar(&c2Bias);
            deletevar(&s2Bias);
            deletevar(&c3Bias);
            deletevar(&c4Bias);
            deletevar(&c5Bias);
            deletevar(&s5Bias);
            deletevar(&f1Bias);
            deletevar(&f2Bias);
            deletevar(&f3Bias);

            deletevar(&c1MapDt);
            deletevar(&s1MapDt);
            deletevar(&c2MapDt);
            deletevar(&s2MapDt);
            deletevar(&c3MapDt);
            deletevar(&c4MapDt);
            deletevar(&c5MapDt);
            deletevar(&s5MapDt);
            deletevar(&f1MapDt);
            deletevar(&f2MapDt);
            deletevar(&f3MapDt);

            deletevar(&c1ConvDt);
            deletevar(&s1PoolDt);
            deletevar(&c2ConvDt);
            deletevar(&s2PoolDt);
            deletevar(&c3ConvDt);
            deletevar(&c4ConvDt);
            deletevar(&c5ConvDt);
            deletevar(&s5PoolDt);
            deletevar(&f1ConnDt);
            deletevar(&f2ConnDt);
            deletevar(&f3ConnDt);

            deletevar(&c1BiasDt);
            deletevar(&s1BiasDt);
            deletevar(&c2BiasDt);
            deletevar(&s2BiasDt);
            deletevar(&c3BiasDt);
            deletevar(&c4BiasDt);
            deletevar(&c5BiasDt);
            deletevar(&s5BiasDt);
            deletevar(&f1BiasDt);
            deletevar(&f2BiasDt);
            deletevar(&f3BiasDt);

            deletevar(&c1ConvEDt);
            deletevar(&s1PoolEDt);
            deletevar(&c2ConvEDt);
            deletevar(&s2PoolEDt);
            deletevar(&c3ConvEDt);
            deletevar(&c4ConvEDt);
            deletevar(&c5ConvEDt);
            deletevar(&s5PoolEDt);
            deletevar(&f1ConnEDt);
            deletevar(&f2ConnEDt);
            deletevar(&f3ConnEDt);

            deletevar(&c1BiasEDt);
            deletevar(&s1BiasEDt);
            deletevar(&c2BiasEDt);
            deletevar(&s2BiasEDt);
            deletevar(&c3BiasEDt);
            deletevar(&c4BiasEDt);
            deletevar(&c5BiasEDt);
            deletevar(&s5BiasEDt);
            deletevar(&f1BiasEDt);
            deletevar(&f2BiasEDt);
            deletevar(&f3BiasEDt);
        }

        void slqAlexNet::init()
        {
            initParm();
        }

        void slqAlexNet::train()
        {
            int epo;
            int ipo = 0;
            float acc = 0.0f;
            ifstream trainStream;
            ifstream trainlabel;

            for (epo = 0; epo < EpochLoop; epo++)
            {
                trainStream.open("imgTrainArray", ifstream::in | ifstream::binary);
                trainlabel.open("imgTrainLabel", ifstream::in | ifstream::binary);
                if ((!trainStream) || (!trainlabel))
                {
                    if (trainStream)
                        trainStream.close();
                    if (trainlabel)
                        trainlabel.close();
                    cout << "There does not exist train data" << endl;
                    return;
                }

                while (EOF != trainStream.peek())
                {
                    trainStream.read(inRaw, sizeof(char)*inUnitNum);
                    trainlabel.read(&curLabl, sizeof(char));

                    //Mat img(227, 227, CV_8UC1);
                    //memcpy((char*)img.data, inRaw, 227*227);
                    //cv::namedWindow("inRaw");
                    //cv::imshow("inRaw", img);
                    //cv::waitKey(0);

                    //memcpy((char*)img.data, inRaw + 227*227, 227 * 227);
                    //cv::namedWindow("inRaw");
                    //cv::imshow("inRaw", img);
                    //cv::waitKey(0);

                    //memcpy((char*)img.data, inRaw + 227*227*2, 227 * 227);
                    //cv::namedWindow("inRaw");
                    //cv::imshow("inRaw", img);
                    //cv::waitKey(0);

                    ProduceLabel();
                    RegularMap(inRaw, inMap);

                    ForwardC1();
                    ForwardS1();
                    ForwardC2();
                    ForwardS2();
                    ForwardC3();
                    ForwardC4();
                    ForwardC5();
                    ForwardS5();
                    ForwardF1();
                    ForwardF2();
                    ForwardF3();

                    BackwardF3();
                    BackwardF2();
                    BackwardF1();
                    BackwardS5();
                    BackwardC5();
                    BackwardC4();
                    BackwardC3();
                    BackwardS2();
                    BackwardC2();
                    BackwardS1();
                    BackwardC1();

                    UpgradeNetwork();

                    ipo++;
                }

                trainlabel.close();
                trainStream.close();

                acc = test();
                cout << "epoch " << epo << " accuracy " << acc << endl;

                if (acc >= AccuracyRate)
                {
                    cout << "Save Params ..." << endl;
                    SaveParameters();
                }
                else if (acc < 0)
                {
                    return;
                }
            }

            if (EpochLoop == epo)
            {
                cout << "End loop, Save Params" << endl;
                SaveParameters();
            }

        }

        void slqAlexNet::CreateConv3Table()
        {
            int oIdx;
            int iIdx;
#define AO true
#define AX false
            for (oIdx = 0; oIdx < c3ConvNum; oIdx++)
            {
                for (iIdx = 0; iIdx < c3ConvDeep; iIdx++)
                {
                    if ((iIdx > oIdx) && (iIdx < oIdx + 5))
                    {
                        CONV3Table[oIdx][iIdx] = AX;
                        continue;
                    }
                    CONV3Table[oIdx][iIdx] = AO;
                }
            }
#undef AO
#undef AX
        }


        void slqAlexNet::deletevar(float **var)
        {
            if (nullptr != *var)
            {
                delete *var;
                *var = nullptr;
            }
        }


        void slqAlexNet::initParm()
        {
            newParam();

            // all weights follow gauss distribution
            uniform_rand(c1Conv, c1ConvUNum, 0.f, 0.f);
            uniform_rand(s1Pool, s1MapNum, 0.f, 0.f);
            uniform_rand(c2Conv, c2ConvUNum, 0.f, 0.f);
            uniform_rand(s2Pool, s2MapNum, 0.f, 0.f);
            uniform_rand(c3Conv, c3ConvUNum, 0.f, 0.f);
            uniform_rand(c4Conv, c4ConvUNum, 0.f, 0.f);
            uniform_rand(c5Conv, c5ConvUNum, 0.f, 0.f);
            uniform_rand(s5Pool, s5MapNum, 0.f, 0.f);
            uniform_rand(f1Conn, f1ConnNum, 0.f, 0.f);
            uniform_rand(f2Conn, f2ConnNum, 0.f, 0.f);
            uniform_rand(f3Conn, f3ConnNum, 0.f, 0.f);

            // 2th, 4th, 5th conv bias set to 1
            //std::fill(c2Bias, c2Bias + c2MapNum, 1.0f);
            //std::fill(c4Bias, c4Bias + c4MapNum, 1.0f);
            //std::fill(c5Bias, c5Bias + c5MapNum, 1.0f);

        }


        void slqAlexNet::newParam()
        {
            mlabel = new float[f3UnitNum]();
            inRaw = new char[inUnitNum]();
            inMap = new float[inUnitNum]();
            c1Map = new float[c1UnitNum]();
            s1Map = new float[s1UnitNum]();
            c2Map = new float[c2UnitNum]();
            s2Map = new float[s2UnitNum]();
            c3Map = new float[c3UnitNum]();
            c4Map = new float[c4UnitNum]();
            c5Map = new float[c5UnitNum]();
            s5Map = new float[s5UnitNum]();
            f1Map = new float[f1UnitNum]();
            f2Map = new float[f2UnitNum]();
            f3Map = new float[f3UnitNum]();

            c1Conv = new float[c1ConvUNum]();
            s1Pool = new float[s1MapNum]();
            c2Conv = new float[c2ConvUNum]();
            s2Pool = new float[s2MapNum]();
            c3Conv = new float[c3ConvUNum]();
            c4Conv = new float[c4ConvUNum]();
            c5Conv = new float[c5ConvUNum]();
            s5Pool = new float[s5MapNum]();
            f1Conn = new float[f1ConnNum]();
            f2Conn = new float[f2ConnNum]();
            f3Conn = new float[f3ConnNum]();

            c1Bias = new float[c1MapNum]();
            s1Bias = new float[s1MapNum]();
            c2Bias = new float[c2MapNum]();
            s2Bias = new float[s2MapNum]();
            c3Bias = new float[c3MapNum]();
            c4Bias = new float[c4MapNum]();
            c5Bias = new float[c5MapNum]();
            s5Bias = new float[s5MapNum]();
            f1Bias = new float[f1UnitNum]();
            f2Bias = new float[f2UnitNum]();
            f3Bias = new float[f3UnitNum]();

            c1MapDt = new float[c1UnitNum]();
            s1MapDt = new float[s1UnitNum]();
            c2MapDt = new float[c2UnitNum]();
            s2MapDt = new float[s2UnitNum]();
            c3MapDt = new float[c3UnitNum]();
            c4MapDt = new float[c4UnitNum]();
            c5MapDt = new float[c5UnitNum]();
            s5MapDt = new float[s5UnitNum]();
            f1MapDt = new float[f1UnitNum]();
            f2MapDt = new float[f2UnitNum]();
            f3MapDt = new float[f3UnitNum]();

            c1ConvDt = new float[c1ConvUNum]();
            s1PoolDt = new float[s1MapNum]();
            c2ConvDt = new float[c2ConvUNum]();
            s2PoolDt = new float[s2MapNum]();
            c3ConvDt = new float[c3ConvUNum]();
            c4ConvDt = new float[c4ConvUNum]();
            c5ConvDt = new float[c5ConvUNum]();
            s5PoolDt = new float[s5MapNum]();
            f1ConnDt = new float[f1ConnNum]();
            f2ConnDt = new float[f2ConnNum]();
            f3ConnDt = new float[f3ConnNum]();


            c1BiasDt = new float[c1MapNum]();
            s1BiasDt = new float[s1MapNum]();
            c2BiasDt = new float[c2MapNum]();
            s2BiasDt = new float[s2MapNum]();
            c3BiasDt = new float[c3MapNum]();
            c4BiasDt = new float[c4MapNum]();
            c5BiasDt = new float[c5MapNum]();
            s5BiasDt = new float[s5MapNum]();
            f1BiasDt = new float[f1UnitNum]();
            f2BiasDt = new float[f2UnitNum]();
            f3BiasDt = new float[f3UnitNum]();

            c1ConvEDt = new float[c1ConvUNum]();
            s1PoolEDt = new float[s1MapNum]();
            c2ConvEDt = new float[c2ConvUNum]();
            s2PoolEDt = new float[s2MapNum]();
            c3ConvEDt = new float[c3ConvUNum]();
            c4ConvEDt = new float[c4ConvUNum]();
            c5ConvEDt = new float[c5ConvUNum]();
            s5PoolEDt = new float[s5MapNum]();
            f1ConnEDt = new float[f1ConnNum]();
            f2ConnEDt = new float[f2ConnNum]();
            f3ConnEDt = new float[f3ConnNum]();

            c1BiasEDt = new float[c1MapNum]();
            s1BiasEDt = new float[s1MapNum]();
            c2BiasEDt = new float[c2MapNum]();
            s2BiasEDt = new float[s2MapNum]();
            c3BiasEDt = new float[c3MapNum]();
            c4BiasEDt = new float[c4MapNum]();
            c5BiasEDt = new float[c5MapNum]();
            s5BiasEDt = new float[s5MapNum]();
            f1BiasEDt = new float[f1UnitNum]();
            f2BiasEDt = new float[f2UnitNum]();
            f3BiasEDt = new float[f3UnitNum]();
        }


        void slqAlexNet::ForwardC1()
        {
            int params[] = {inMapNum, c1MapNum, inMapHigh, inMapWidth, c1MapHigh, c1MapWidth, c1ConvHigh, c1ConvWidth, c1ConvStride, 0 };

            std::fill(c1Map, c1Map + c1UnitNum, 0.f);
            ConvolutionOpt(inMap, c1Map, c1Conv, c1Bias, params);
        }


        void slqAlexNet::ForwardS1()
        {
            int params[] = {s1MapNum, c1MapHigh, c1MapWidth, s1MapHigh, s1MapWidth, poolSpace, poolStride, 2};

            float tmp[c1UnitNum];
            memcpy((char*)tmp, (char*)c1Map, sizeof(float)*c1UnitNum);
            
            std::fill(s1Map, s1Map + s1UnitNum, 0.f);
            PoolingOpt(c1Map, s1Map, s1Pool, s1Bias, params);
        }


        void slqAlexNet::ForwardC2()
        {
            int params[] = { s1MapNum / 2, c2MapNum / 2, s1MapHigh, s1MapWidth, c2MapHigh, c2MapWidth, c2ConvHigh, c2ConvWidth, c2ConvStride, 0 };

            float tmp[s1UnitNum];
            memcpy((char*)tmp, (char*)s1Map, sizeof(float)*s1UnitNum);

            std::fill(c2Map, c2Map + c2UnitNum, 0.f);
            ConvolutionOpt(s1Map, c2Map, c2Conv, c2Bias, params);

            ConvolutionOpt(s1Map + s1MapNum / 2 * s1MapSize, c2Map + c2MapNum / 2 * c2MapSize, c2Conv + c2MapNum / 2 * c2ConvSize, c2Bias + c2MapNum / 2, params);

        }


        void slqAlexNet::ForwardS2()
        {
            int params[] = { s2MapNum, c2MapHigh, c2MapWidth, s2MapHigh, s2MapWidth, poolSpace, poolStride, 1 };

            float tmp[c2UnitNum];
            memcpy((char*)tmp, (char*)c2Map, sizeof(float)*c2UnitNum);

            std::fill(s2Map, s2Map + s2UnitNum, 0.f);
            PoolingOpt(c2Map, s2Map, s2Pool, s2Bias, params);
        }


        void slqAlexNet::ForwardC3()
        {
            int params[] = { s2MapNum, c3MapNum, s2MapHigh, s2MapWidth, c3MapHigh, c3MapWidth, c3ConvHigh, c3ConvWidth, c3ConvStride, 1 };

            float tmp[s2UnitNum];
            memcpy((char*)tmp, (char*)s2Map, sizeof(float)*s2UnitNum);

            std::fill(c3Map, c3Map + c3UnitNum, 0.f);
            ConvolutionOpt(s2Map, c3Map, c3Conv, c3Bias, params);            

        }


        void slqAlexNet::ForwardC4()
        {
            int params[] = { c3MapNum / 2, c4MapNum / 2, c3MapHigh, c3MapWidth, c4MapHigh, c4MapWidth, c4ConvHigh, c4ConvWidth, c4ConvStride, 1 };

            float tmp[c3UnitNum];
            memcpy((char*)tmp, (char*)c3Map, sizeof(float)*c3UnitNum);

            std::fill(c4Map, c4Map + c4UnitNum, 0.f);
            ConvolutionOpt(c3Map, c4Map, c4Conv, c4Bias, params);

            ConvolutionOpt(c3Map + c3MapNum / 2 * c3MapSize, c4Map + c4MapNum / 2 * c4MapSize, c4Conv + c4MapNum / 2 * c4ConvSize, c4Bias + c4MapNum / 2, params);

        }


        void slqAlexNet::ForwardC5()
        {
            int params[] = { c4MapNum / 2, c5MapNum / 2, c4MapHigh, c4MapWidth, c5MapHigh, c5MapWidth, c5ConvHigh, c5ConvWidth, c5ConvStride, 0 };

            float tmp[c4UnitNum];
            memcpy((char*)tmp, (char*)c4Map, sizeof(float)*c4UnitNum);

            std::fill(c5Map, c5Map + c5UnitNum, 0.f);
            ConvolutionOpt(c4Map, c5Map, c5Conv, c5Bias, params);

            ConvolutionOpt(c4Map + c4MapNum / 2 * c4MapSize, c5Map + c5MapNum / 2 * c5MapSize, c5Conv + c5MapNum / 2 * c5ConvSize, c5Bias + c5MapNum / 2, params);

        }


        void slqAlexNet::ForwardS5()
        {
            int params[] = { s5MapNum, c5MapHigh, c5MapWidth, s5MapHigh, s5MapWidth, poolSpace, poolStride, 0 };

            float tmp[c5UnitNum];
            memcpy((char*)tmp, (char*)c5Map, sizeof(float)*c5UnitNum);

            std::fill(s5Map, s5Map + s5UnitNum, 0.f);
            PoolingOpt(c5Map, s5Map, s5Pool, s5Bias, params);

        }


        void slqAlexNet::ForwardF1()
        {
            int oIdx;
            int iIdx;

            float tmp[s5UnitNum];
            memcpy((char*)tmp, (char*)s5Map, sizeof(float)*s5UnitNum);

            std::fill(f1Map, f1Map + f1UnitNum, 0.f);
            for (oIdx = 0; oIdx < f1UnitNum; oIdx++)
            {
                float *curmap = f1Map + oIdx;
                for (iIdx = 0; iIdx < s5UnitNum; iIdx++)
                {
                    *curmap += s5Map[iIdx] * f1Conn[iIdx * f1UnitNum + oIdx];
                }

                *curmap += f1Bias[oIdx];
                *curmap = ACTIVATION(*curmap);
            }
        }


        void slqAlexNet::ForwardF2()
        {
            int oIdx;
            int iIdx;


            float tmp[f1UnitNum];
            memcpy((char*)tmp, (char*)f1Map, sizeof(float)*f1UnitNum);

            std::fill(f2Map, f2Map + f2UnitNum, 0.f);
            for (oIdx = 0; oIdx < f2UnitNum; oIdx++)
            {
                float *curmap = f2Map + oIdx;
                for (iIdx = 0; iIdx < f1UnitNum; iIdx++)
                {
                    *curmap += f1Map[iIdx] * f2Conn[iIdx * f2UnitNum + oIdx];
                }

                *curmap += f2Bias[oIdx];
                *curmap = ACTIVATION(*curmap);
            }
        }


        void slqAlexNet::ForwardF3()
        {
            int oIdx;
            int iIdx;


            float tmp[f2UnitNum];
            memcpy((char*)tmp, (char*)f2Map, sizeof(float)*f2UnitNum);


            std::fill(f3Map, f3Map + f3UnitNum, 0.f);
            for (oIdx = 0; oIdx < f3UnitNum; oIdx++)
            {
                float *curmap = f3Map + oIdx;
                for (iIdx = 0; iIdx < f2UnitNum; iIdx++)
                {
                    *curmap += f2Map[iIdx] * f3Conn[iIdx * f3UnitNum + oIdx];
                }

                *curmap += f3Bias[oIdx];
                *curmap = ACTIVATION(*curmap);
            }
        }


        void slqAlexNet::BackwardF3()
        {
            int oIdx;
            int iIdx;

            float tmpf3[f3UnitNum];
            memcpy((char*)tmpf3, (char*)f3Map, sizeof(float)*f3UnitNum);

            for (oIdx = 0; oIdx < f3UnitNum; oIdx++)
            {
                f3MapDt[oIdx] = (f3Map[oIdx] - mlabel[oIdx]) * ACTDEVICE(f3Map[oIdx]);
                f3BiasDt[oIdx] = f3MapDt[oIdx];


                for (iIdx = 0; iIdx < f2UnitNum; iIdx++)
                {
                    f3ConnDt[iIdx*f3UnitNum + oIdx] = f3MapDt[oIdx] * f2Map[iIdx];
                }
            }


        }


        void slqAlexNet::BackwardF2()
        {
            int oIdx;
            int iIdx;

            for (oIdx = 0; oIdx < f2UnitNum; oIdx++)
            {
                float curdt = 0.f;
                for (iIdx = 0; iIdx < f3UnitNum; iIdx++)
                {
                    curdt += f3MapDt[iIdx] * f3Conn[oIdx * f3UnitNum + iIdx];
                }

                f2MapDt[oIdx] = ACTDEVICE(f2Map[oIdx]) * curdt;
                f2BiasDt[oIdx] = f2MapDt[oIdx];


                for (iIdx = 0; iIdx < f1UnitNum; iIdx++)
                {
                    f2ConnDt[iIdx * f2UnitNum + oIdx] = f2MapDt[oIdx] * f1Map[iIdx];
                }
            }


        }


        void slqAlexNet::BackwardF1()
        {
            int oIdx;
            int iIdx;

            for (oIdx = 0; oIdx < f1UnitNum; oIdx++)
            {
                float curdt = 0.f;
                for (iIdx = 0; iIdx < f2UnitNum; iIdx++)
                {
                    curdt += f2MapDt[iIdx] * f2Conn[oIdx * f2UnitNum + iIdx];
                }

                f1MapDt[oIdx] = ACTDEVICE(f1Map[oIdx]) * curdt;
                f1BiasDt[oIdx] = f1MapDt[oIdx];


                for (iIdx = 0; iIdx < s5UnitNum; iIdx++)
                {
                    f1ConnDt[iIdx * f1UnitNum + oIdx] = f1MapDt[oIdx] * s5Map[iIdx];
                }
            }


        }


        void slqAlexNet::BackwardS5()
        {
            int iIdx;
            int iod;

            int odeepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            std::fill(s5BiasDt, s5BiasDt + s5MapNum, 0.f);
            std::fill(s5PoolDt, s5PoolDt + s5MapNum, 0.f);
            for (odeepIdx = 0; odeepIdx < s5MapNum; odeepIdx++)
            {
                for (ohIdx = 0; ohIdx < s5MapHigh; ohIdx++)
                {
                    iod = odeepIdx*s5MapSize + ohIdx * s5MapWidth;
                    for (owIdx = 0; owIdx < s5MapWidth; owIdx++)
                    {
                        float curdt = 0.f;
                        for (iIdx = 0; iIdx < f1UnitNum; iIdx++)
                        {
                            curdt += f1MapDt[iIdx] * f1Conn[(iod + owIdx)*f1UnitNum + iIdx];
                        }

                        s5MapDt[iod + owIdx] = ACTDEVICE(s5Map[iod + owIdx]) * curdt;
                        s5BiasDt[odeepIdx] += s5MapDt[iod + owIdx];

                        float *curin = c5Map + odeepIdx * c5MapSize + ohIdx * poolStride * c5MapWidth + owIdx * poolStride;
                        curdt = 0.f;
                        for (phIdx = 0; phIdx < poolSpace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolSpace; pwIdx++)
                            {
                                curdt += *(curin + phIdx * c5MapWidth + pwIdx);
                            }
                        }

                        s5PoolDt[odeepIdx] += curdt / (poolSpace * poolSpace) * s5MapDt[iod + owIdx];
                    }
                }
            }

        }


        void slqAlexNet::BackwardC5()
        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            std::fill(c5MapDt, c5MapDt + c5UnitNum, 0.f);
            std::fill(c5BiasDt, c5BiasDt + c5MapNum, 0.f);
            std::fill(c5ConvDt, c5ConvDt + c5ConvUNum, 0.f);

            for (odeepIdx = 0; odeepIdx < c5MapNum; odeepIdx++)
            {
                for (ohIdx = 0; ohIdx < s5MapHigh; ohIdx++)
                {
                    for (owIdx = 0; owIdx < s5MapWidth; owIdx++)
                    {
                        for (phIdx = 0; phIdx < poolSpace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolSpace; pwIdx++)
                            {
                                iod = odeepIdx * c5MapSize + (ohIdx * poolStride + phIdx) * c5MapWidth + (owIdx * poolStride + pwIdx);
                                vod = odeepIdx * s5MapSize + ohIdx * s5MapWidth + owIdx;
                                c5MapDt[iod] += ACTDEVICE(c5Map[iod]) * s5Pool[odeepIdx] * s5MapDt[vod] / (poolSpace * poolSpace);
                                c5BiasDt[odeepIdx] += c5MapDt[iod];
                            }
                        }
                    }
                }
            }

            for (odeepIdx = 0; odeepIdx < c5MapNum / 2; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c5ConvDeep; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c5ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c5ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c5ConvSize + ideepIdx * c5ConvTensor + phIdx * c5ConvWidth + pwIdx;
                            float *curconv = c5ConvDt + iod;

                            for (ohIdx = 0; ohIdx < c5MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c5MapWidth; owIdx++)
                                {
                                    svh = ideepIdx * c4MapSize + (phIdx + ohIdx * c5ConvStride)*c4MapWidth + (pwIdx + owIdx * c5ConvStride);
                                    vod = odeepIdx * c5MapSize + ohIdx * c5MapWidth + owIdx;
                                    *curconv += c4Map[svh] * c5MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = c5MapNum / 2; odeepIdx < c5MapNum; odeepIdx++)
            {
                for (ideepIdx = c5ConvDeep; ideepIdx < c4MapNum; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c5ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c5ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c5ConvSize + (ideepIdx - c5ConvDeep) * c5ConvTensor + phIdx * c5ConvWidth + pwIdx;
                            float *curconv = c5ConvDt + iod;

                            for (ohIdx = 0; ohIdx < c5MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c5MapWidth; owIdx++)
                                {
                                    svh = ideepIdx * c4MapSize + (phIdx + ohIdx * c5ConvStride)*c4MapWidth + (pwIdx + owIdx * c5ConvStride);
                                    vod = odeepIdx * c5MapSize + ohIdx * c5MapWidth + owIdx;
                                    *curconv += c4Map[svh] * c5MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }
        }


        void slqAlexNet::BackwardC4()
        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;


            std::fill(c4MapDt, c4MapDt + c4UnitNum, 0.f);
            std::fill(c4BiasDt, c4BiasDt + c4MapNum, 0.f);
            std::fill(c4ConvDt, c4ConvDt + c4ConvUNum, 0.f);
            for (odeepIdx = 0; odeepIdx < c4MapNum / 2; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c5MapNum / 2; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c5ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c5ConvWidth; pwIdx++)
                        {
                            svh = ideepIdx * c5ConvSize + odeepIdx * c5ConvTensor + phIdx * c5ConvWidth + pwIdx;
                            for (ohIdx = 0; ohIdx < c5MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c5MapWidth; owIdx++)
                                {
                                    iod = odeepIdx * c4MapSize + (phIdx + ohIdx * c5ConvStride) * c4MapWidth + (pwIdx + owIdx * c5ConvStride);
                                    vod = ideepIdx * c5MapSize + ohIdx * c5MapWidth + owIdx;
                                    c4MapDt[iod] += ACTDEVICE(c4Map[iod]) * c5Conv[svh] * c5MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = c4MapNum / 2; odeepIdx < c4MapNum; odeepIdx++)
            {
                for (ideepIdx = c5MapNum / 2; ideepIdx < c5MapNum; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c5ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c5ConvWidth; pwIdx++)
                        {
                            svh = ideepIdx * c5ConvSize + (odeepIdx - c5ConvDeep) * c5ConvTensor + phIdx * c5ConvWidth + pwIdx;
                            for (ohIdx = 0; ohIdx < c5MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c5MapWidth; owIdx++)
                                {
                                    iod = odeepIdx * c4MapSize + (phIdx + ohIdx * c5ConvStride) * c4MapWidth + (pwIdx + owIdx * c5ConvStride);
                                    vod = ideepIdx * c5MapSize + ohIdx * c5MapWidth + owIdx;
                                    c4MapDt[iod] += ACTDEVICE(c4Map[iod]) * c5Conv[svh] * c5MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = 0; odeepIdx < c4MapNum / 2; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c4ConvDeep; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c4ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c4ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c4ConvSize + ideepIdx * c4ConvTensor + phIdx * c4ConvWidth + pwIdx;
                            for (ohIdx = 1; ohIdx < c4MapHigh - 1; ohIdx++)
                            {
                                for (owIdx = 1; owIdx < c4MapWidth - 1; owIdx++)
                                {
                                    vod = odeepIdx * c4MapSize + ohIdx * c4MapWidth + owIdx;
                                    svh = ideepIdx * c3MapSize + ((ohIdx - 1) * c4ConvStride + phIdx) * c3MapWidth + ((owIdx - 1) * c4ConvStride + pwIdx);
                                    c4BiasDt[odeepIdx] += c4MapDt[vod];
                                    c4ConvDt[iod] += c3Map[svh] * c4MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = c4MapNum / 2; odeepIdx < c4MapNum; odeepIdx++)
            {
                for (ideepIdx = c4ConvDeep; ideepIdx < c3MapNum; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c4ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c4ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c4ConvSize + (ideepIdx - c4ConvDeep) * c4ConvTensor + phIdx * c4ConvWidth + pwIdx;
                            for (ohIdx = 1; ohIdx < c4MapHigh - 1; ohIdx++)
                            {
                                for (owIdx = 1; owIdx < c4MapWidth - 1; owIdx++)
                                {
                                    vod = odeepIdx * c4MapSize + ohIdx * c4MapWidth + owIdx;
                                    svh = ideepIdx * c3MapSize + ((ohIdx - 1)  * c4ConvStride + phIdx) * c3MapWidth + ((owIdx - 1) * c4ConvStride + pwIdx);
                                    c4BiasDt[odeepIdx] += c4MapDt[vod];
                                    c4ConvDt[iod] += c3Map[svh] * c4MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

        }


        void slqAlexNet::BackwardC3()
        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            std::fill(c3MapDt, c3MapDt + c3UnitNum, 0.f);
            std::fill(c3BiasDt, c3BiasDt + c3MapNum, 0.f);
            std::fill(c3ConvDt, c3ConvDt + c3ConvUNum, 0.f);
            for (odeepIdx = 0; odeepIdx < c3MapNum / 2; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c4MapNum / 2; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c4ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c4ConvWidth; pwIdx++)
                        {
                            svh = ideepIdx * c4ConvSize + odeepIdx * c4ConvTensor + phIdx * c4ConvWidth + pwIdx;
                            for (ohIdx = 1; ohIdx < c4MapHigh - 1; ohIdx++)
                            {
                                for (owIdx = 1; owIdx < c4MapWidth - 1; owIdx++)
                                {
                                    iod = odeepIdx * c3MapSize + (phIdx + (ohIdx - 1) * c4ConvStride) * c3MapWidth + (pwIdx + (owIdx - 1) * c4ConvStride);
                                    vod = ideepIdx * c4MapSize + ohIdx * c4MapWidth + owIdx;
                                    c3MapDt[iod] += ACTDEVICE(c3Map[iod]) * c4Conv[svh] * c4MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = c3MapNum / 2; odeepIdx < c3MapNum; odeepIdx++)
            {
                for (ideepIdx = c4MapNum / 2; ideepIdx < c4MapNum; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c4ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c4ConvWidth; pwIdx++)
                        {
                            svh = ideepIdx * c4ConvSize + (odeepIdx - c4ConvDeep) * c4ConvTensor + phIdx * c4ConvWidth + pwIdx;
                            for (ohIdx = 1; ohIdx < c4MapHigh - 1; ohIdx++)
                            {
                                for (owIdx = 1; owIdx < c4MapWidth - 1; owIdx++)
                                {
                                    iod = odeepIdx * c3MapSize + (phIdx + (ohIdx - 1) * c4ConvStride) * c3MapWidth + (pwIdx + (owIdx - 1) * c4ConvStride);
                                    vod = ideepIdx * c4MapSize + ohIdx * c4MapWidth + owIdx;
                                    c3MapDt[iod] += ACTDEVICE(c3Map[iod]) * c4Conv[svh] * c4MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = 0; odeepIdx < c3MapNum; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c3ConvDeep; ideepIdx++)
                {
                    if (!CONV3Table[odeepIdx][ideepIdx])
                        continue;

                    for (phIdx = 0; phIdx < c3ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c3ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c3ConvSize + ideepIdx * c3ConvTensor + phIdx * c3ConvWidth + pwIdx;
                            for (ohIdx = 1; ohIdx < c3MapHigh - 1; ohIdx++)
                            {
                                for (owIdx = 1; owIdx < c3MapWidth - 1; owIdx++)
                                {
                                    vod = odeepIdx * c3MapSize + ohIdx * c3MapWidth + owIdx;
                                    svh = ideepIdx * s2MapSize + ((ohIdx - 1) * c3ConvStride + phIdx) * s2MapWidth + ((owIdx - 1) * c3ConvStride + pwIdx);
                                    c3BiasDt[odeepIdx] += c3MapDt[vod];
                                    c3ConvDt[iod] += c3MapDt[vod] * s2Map[svh];
                                }
                            }
                        }
                    }
                }
            }
        }


        void slqAlexNet::BackwardS2()
        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            std::fill(s2MapDt, s2MapDt + s2UnitNum, 0.f);
            std::fill(s2BiasDt, s2BiasDt + s2MapNum, 0.f);
            std::fill(s2PoolDt, s2PoolDt + s2MapNum, 0.f);
            for (odeepIdx = 0; odeepIdx < s2MapNum; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c3MapNum; ideepIdx++)
                {
                    if (!CONV3Table[ideepIdx][odeepIdx])
                        continue;

                    for (phIdx = 0; phIdx < c3ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c3ConvWidth; pwIdx++)
                        {
                            svh = ideepIdx * c3ConvSize + odeepIdx * c3ConvTensor + phIdx * c3ConvWidth + pwIdx;
                            for (ohIdx = 1; ohIdx < c3MapHigh - 1; ohIdx++)
                            {
                                for (owIdx = 1; owIdx < c3MapWidth - 1; owIdx++)
                                {
                                    iod = odeepIdx * s2MapSize + (phIdx + (ohIdx - 1) * c3ConvStride) * s2MapWidth + (pwIdx + (owIdx - 1) * c3ConvStride);
                                    vod = ideepIdx * c3MapSize + ohIdx * c3MapWidth + owIdx;
                                    s2MapDt[iod] += ACTDEVICE(s2Map[iod]) * c3Conv[svh] * c3MapDt[vod];
                                }
                            }
                        }
                    }
                }
            }


            for (odeepIdx = 0; odeepIdx < s2MapNum; odeepIdx++)
            {
                for (ohIdx = 1; ohIdx < s2MapHigh - 1; ohIdx++)
                {
                    iod = odeepIdx*s2MapSize + ohIdx * s2MapWidth;
                    for (owIdx = 1; owIdx < s2MapWidth - 1; owIdx++)
                    {
                        float *curin = c2Map + odeepIdx * c2MapSize + (ohIdx - 1) * poolStride * c2MapWidth + (owIdx - 1) * poolStride;
                        float curdt = 0.f;
                        for (phIdx = 0; phIdx < poolSpace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolSpace; pwIdx++)
                            {
                                curdt += *(curin + phIdx * c2MapWidth + pwIdx);
                            }
                        }

                        s2BiasDt[odeepIdx] += s2MapDt[iod + owIdx];
                        s2PoolDt[odeepIdx] += curdt / (poolSpace * poolSpace) * s2MapDt[iod + owIdx];

                    }
                }

            }
        }


        void slqAlexNet::BackwardC2()
        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            std::fill(c2MapDt, c2MapDt + c2UnitNum, 0.f);
            std::fill(c2BiasDt, c2BiasDt + c2MapNum, 0.f);
            std::fill(c2ConvDt, c2ConvDt + c2ConvUNum, 0.f);
            for (odeepIdx = 0; odeepIdx < c2MapNum; odeepIdx++)
            {
                for (ohIdx = 1; ohIdx < s2MapHigh - 1; ohIdx++)
                {
                    for (owIdx = 1; owIdx < s2MapWidth - 1; owIdx++)
                    {
                        for (phIdx = 0; phIdx < poolSpace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolSpace; pwIdx++)
                            {
                                iod = odeepIdx * c2MapSize + ((ohIdx - 1) * poolStride + phIdx) * c2MapWidth + ((owIdx - 1) * poolStride + pwIdx);
                                vod = odeepIdx * s2MapSize + ohIdx * s2MapWidth + owIdx;
                                c2MapDt[iod] += ACTDEVICE(c2Map[iod]) * s2Pool[odeepIdx] * s2MapDt[vod] / (poolSpace * poolSpace);
                                c2BiasDt[odeepIdx] += c2MapDt[iod];
                            }
                        }
                    }
                }
            }

            for (odeepIdx = 0; odeepIdx < c2MapNum / 2; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c2ConvDeep; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c2ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c2ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c2ConvSize + ideepIdx * c2ConvTensor + phIdx * c2ConvWidth + pwIdx;
                            for (ohIdx = 0; ohIdx < c2MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c2MapWidth; owIdx++)
                                {
                                    vod = odeepIdx * c2MapSize + ohIdx * c2MapWidth + owIdx;
                                    svh = ideepIdx * s1MapSize + (ohIdx * c2ConvStride + phIdx) * s1MapWidth + (owIdx * c2ConvStride + pwIdx);
                                    c2ConvDt[iod] += c2MapDt[vod] * s1Map[svh];

                                }
                            }
                        }
                    }
                }
            }

            for (odeepIdx = c2MapNum / 2; odeepIdx < c2MapNum; odeepIdx++)
            {
                for (ideepIdx = c2ConvDeep; ideepIdx < s1MapNum; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c2ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c2ConvWidth; pwIdx++)
                        {
                            iod = odeepIdx * c2ConvSize + (ideepIdx - c2ConvDeep) * c2ConvTensor + phIdx * c2ConvWidth + pwIdx;
                            for (ohIdx = 0; ohIdx < c2MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c2MapWidth; owIdx++)
                                {
                                    vod = odeepIdx * c2MapSize + ohIdx * c2MapWidth + owIdx;
                                    svh = ideepIdx * s1MapSize + (ohIdx * c2ConvStride + phIdx) * s1MapWidth + (owIdx * c2ConvStride + pwIdx);
                                    c2ConvDt[iod] += c2MapDt[vod] * s1Map[svh];

                                }
                            }
                        }
                    }
                }
            }
        }


        void slqAlexNet::BackwardS1()
        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            std::fill(s1MapDt, s1MapDt + s1UnitNum, 0.f);
            std::fill(s1BiasDt, s1BiasDt + s1MapNum, 0.f);
            std::fill(s1PoolDt, s1PoolDt + s1MapNum, 0.f);
            for (odeepIdx = 0; odeepIdx < s1MapNum / 2; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c2ConvDeep; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c2ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c2ConvWidth; pwIdx++)
                        {
                            vod = odeepIdx * c2ConvSize + ideepIdx * c2ConvTensor + phIdx * c2ConvHigh + pwIdx;
                            for (ohIdx = 0; ohIdx < c2MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c2MapWidth; owIdx++)
                                {
                                    iod = odeepIdx * s1MapSize + (ohIdx * c2ConvStride + phIdx) * s1MapWidth + (owIdx * c2ConvStride + pwIdx);
                                    svh = ideepIdx * c2MapSize + ohIdx * c2MapWidth + owIdx;
                                    s1MapDt[iod] += ACTDEVICE(s1Map[iod]) * c2Conv[vod] * c2MapDt[svh];
                                }
                            }
                        }
                    }
                }
            }


            for (odeepIdx = s1MapNum / 2; odeepIdx < s1MapNum; odeepIdx++)
            {
                for (ideepIdx = 0; ideepIdx < c2ConvDeep; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c2ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c2ConvWidth; pwIdx++)
                        {
                            vod = odeepIdx * c2ConvSize + ideepIdx * c2ConvTensor + phIdx * c2ConvHigh + pwIdx;
                            for (ohIdx = 0; ohIdx < c2MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c2MapWidth; owIdx++)
                                {
                                    iod = odeepIdx * s1MapSize + (ohIdx * c2ConvStride + phIdx) * s1MapWidth + (owIdx * c2ConvStride + pwIdx);
                                    svh = (ideepIdx + c2ConvDeep) * c2MapSize + ohIdx * c2MapWidth + owIdx;
                                    s1MapDt[iod] += ACTDEVICE(s1Map[iod]) * c2Conv[vod] * c2MapDt[svh];
                                }
                            }
                        }
                    }
                }
            }


            for (odeepIdx = 0; odeepIdx < s1MapNum; odeepIdx++)
            {
                for (ohIdx = 2; ohIdx < s1MapHigh - 2; ohIdx++)
                {
                    iod = odeepIdx*s1MapSize + ohIdx * s1MapWidth;
                    for (owIdx = 2; owIdx < s1MapWidth - 2; owIdx++)
                    {

                        float *curin = c1Map + odeepIdx * c1MapSize + (ohIdx - 2) * poolStride * c1MapWidth + (owIdx - 2) * poolStride;
                        float curdt = 0.f;
                        for (phIdx = 0; phIdx < poolSpace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolSpace; pwIdx++)
                            {
                                curdt += *(curin + phIdx * c1MapWidth + pwIdx);
                            }

                        }

                        s1BiasDt[odeepIdx] += s1MapDt[iod + owIdx];
                        s1PoolDt[odeepIdx] += curdt / (poolSpace * poolSpace) * s1MapDt[iod + owIdx];
                    }
                }
            }

        }


        void slqAlexNet::BackwardC1()

        {
            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            std::fill(c1MapDt, c1MapDt + c1UnitNum, 0.f);
            std::fill(c1BiasDt, c1BiasDt + c1MapNum, 0.f);
            std::fill(c1ConvDt, c1ConvDt + c1ConvUNum, 0.f);
            for (odeepIdx = 0; odeepIdx < c1MapNum; odeepIdx++)
            {
                for (ohIdx = 2; ohIdx < s1MapHigh - 2; ohIdx++)
                {
                    for (owIdx = 2; owIdx < s1MapWidth - 2; owIdx++)
                    {
                        for (phIdx = 0; phIdx < poolSpace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolSpace; pwIdx++)
                            {
                                iod = odeepIdx * c1MapSize + ((ohIdx - 2) * poolStride + phIdx) * c1MapWidth + ((owIdx - 2) * poolStride + pwIdx);
                                vod = odeepIdx * s1MapSize + ohIdx * s1MapWidth + owIdx;
                                c1MapDt[iod] += ACTDEVICE(c1Map[iod]) * s1Pool[odeepIdx] * s1MapDt[vod] / (poolSpace * poolSpace);
                                c1BiasDt[odeepIdx] += c1MapDt[iod];
                            }
                        }
                    }
                }

                for (ideepIdx = 0; ideepIdx < c1ConvDeep; ideepIdx++)
                {
                    for (phIdx = 0; phIdx < c1ConvHigh; phIdx++)
                    {
                        for (pwIdx = 0; pwIdx < c1ConvWidth; pwIdx++)
                        {
                            vod = odeepIdx * c1ConvSize + ideepIdx * c1ConvTensor + phIdx * c1ConvWidth + pwIdx;

                            for (ohIdx = 0; ohIdx < c1MapHigh; ohIdx++)
                            {
                                for (owIdx = 0; owIdx < c1MapWidth; owIdx++)
                                {
                                    iod = ideepIdx * inMapSize + (phIdx + ohIdx * c1ConvStride) * inMapWidth + (pwIdx + owIdx * c1ConvStride);
                                    svh = odeepIdx * c1MapSize + ohIdx * c1MapWidth + owIdx;
                                    c1ConvDt[vod] += inMap[iod] * c1MapDt[svh];
                                }
                            }
                        }
                    }

                }

            }


        }



        void slqAlexNet::UpgradeNetwork()
        {
            UpdateParameters(c1ConvDt, c1ConvEDt, c1Conv, c1ConvUNum);
            UpdateParameters(c1BiasDt, c1BiasEDt, c1Bias, c1MapNum);

            UpdateParameters(s1PoolDt, s1PoolEDt, s1Pool, s1MapNum);
            UpdateParameters(s1BiasDt, s1BiasEDt, s1Bias, s1MapNum);

            UpdateParameters(c2ConvDt, c2ConvEDt, c2Conv, c2ConvUNum);
            UpdateParameters(c2BiasDt, c2BiasEDt, c2Bias, c2MapNum);

            UpdateParameters(s2PoolDt, s2PoolEDt, s2Pool, s2MapNum);
            UpdateParameters(s2BiasDt, s2BiasEDt, s2Bias, s2MapNum);

            UpdateParameters(c3ConvDt, c3ConvEDt, c3Conv, c3ConvUNum);
            UpdateParameters(c3BiasDt, c3BiasEDt, c3Bias, c3MapNum);

            UpdateParameters(c4ConvDt, c4ConvEDt, c4Conv, c4ConvUNum);
            UpdateParameters(c4BiasDt, c4BiasEDt, c4Bias, c4MapNum);

            UpdateParameters(c5ConvDt, c5ConvEDt, c5Conv, c5ConvUNum);
            UpdateParameters(c5BiasDt, c5BiasEDt, c5Bias, c5MapNum);

            UpdateParameters(s5PoolDt, s5PoolEDt, s5Pool, s5MapNum);
            UpdateParameters(s5BiasDt, s5BiasEDt, s5Bias, s5MapNum);

            UpdateParameters(f1ConnDt, f1ConnEDt, f1Conn, f1ConnNum);
            UpdateParameters(f1BiasDt, f1BiasEDt, f1Bias, f1UnitNum);

            UpdateParameters(f2ConnDt, f2ConnEDt, f2Conn, f2ConnNum);
            UpdateParameters(f2BiasDt, f2BiasEDt, f2Bias, f2UnitNum);

            UpdateParameters(f3ConnDt, f3ConnEDt, f3Conn, f3ConnNum);
            UpdateParameters(f3BiasDt, f3BiasEDt, f3Bias, f3UnitNum);
        }


        void slqAlexNet::UpdateParameters(float *delta, float *Edelta, float *para, int len)
        {
            for (int lIdx = 0; lIdx < len; lIdx++)
            {
                Edelta[lIdx] += delta[lIdx] * delta[lIdx];
                para[lIdx] -= Alpha * delta[lIdx] / (std::sqrt(Edelta[lIdx]) + EspCNN);
            }
        }

        void slqAlexNet::ConvolutionOpt(float *inPtr, float *outPtr, float *convPtr, float *biPtr, int param[])
        {
            int inMapNo     = param[0];
            int outMapNo    = param[1];
            int inMapH      = param[2];
            int inMapW      = param[3];
            int outMapH     = param[4];
            int outMapW     = param[5];
            int convH       = param[6];
            int convW       = param[7];
            int convStride  = param[8];
            int expand      = param[9];

            int odeepIdx;
            int ideepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;
            int svh;

            int insize = inMapH * inMapW;
            int convsize = inMapNo * convH * convW;
            int convtsor = convH * convW;

            for (odeepIdx = 0; odeepIdx < outMapNo; odeepIdx++)
            {
                for (ohIdx = expand; ohIdx < outMapH - expand; ohIdx++)
                {
                    iod = odeepIdx * outMapH * outMapW + ohIdx * outMapW;
                    for (owIdx = expand; owIdx < outMapW - expand; owIdx++)
                    {
                        float *curmap = outPtr + iod + owIdx;
                        float cur = 0.f;

                        for (ideepIdx = 0; ideepIdx < inMapNo; ideepIdx++)
                        {
                            if ((inMapNo == s2MapNum) && (outMapNo == c3MapNum) && (!CONV3Table[odeepIdx][ideepIdx]))
                                continue;

                            vod = odeepIdx * convsize + ideepIdx * convtsor;
                            svh = ideepIdx * insize + (ohIdx - expand) * convStride * inMapW + (owIdx - expand) * convStride;

                            float *curcv = convPtr + vod;
                            float *curin = inPtr + svh;

                            for (phIdx = 0; phIdx < convH; phIdx++)
                            {
                                for (pwIdx = 0; pwIdx < convW; pwIdx++)
                                {
                                    
                                    cur += *(curcv + phIdx * convW + pwIdx) * (*(curin + phIdx * inMapW + pwIdx));
                                }
                            }
                        }

                        *curmap = cur + *(biPtr + odeepIdx);
                        *curmap = ACTIVATION(*curmap);
                    }
                }
            }

        }

        void slqAlexNet::PoolingOpt(float *inPtr, float *outPtr, float *poolPtr, float *biPtr, int param[])
        {
            int mapNo       = param[0];
            int imapH       = param[1];
            int imapW       = param[2];
            int omapH       = param[3];
            int omapW       = param[4];
            int poolspace   = param[5];
            int poolstride  = param[6];
            int expand      = param[7];

            int deepIdx;
            int ohIdx;
            int owIdx;
            int phIdx;
            int pwIdx;

            int iod;
            int vod;

            for(deepIdx = 0; deepIdx < mapNo; deepIdx++)
            {
                for (ohIdx = expand; ohIdx < omapH - expand; ohIdx++)
                {
                    iod = deepIdx * omapH * omapW + ohIdx * omapW;
                    for (owIdx = expand; owIdx < omapW - expand; owIdx++)
                    {
                        vod = deepIdx * imapH * imapW + (ohIdx - expand) * poolstride * imapW + (owIdx - expand) * poolstride;
                        float *curmap = outPtr + iod + owIdx;
                        float *curin = inPtr + vod;
                        float cur = 0.f;
                        for (phIdx = 0; phIdx < poolspace; phIdx++)
                        {
                            for (pwIdx = 0; pwIdx < poolspace; pwIdx++)
                            {
                                cur += *(curin + phIdx * imapW + pwIdx);
                            }
                        }

                        *curmap = cur / (poolspace * poolspace) + *(biPtr + deepIdx);
                        *curmap = ACTIVATION(*curmap);
                    }
                }
            }
        }


        float slqAlexNet::test()
        {
            float acc = 0.f;
            float prc = 0.f;
            float maxv = -10.f;
            int maxI;
            char tlabl;
            int mIdx;
            int count = 0;
            ifstream testStream;
            ifstream testlabl;

            testStream.open("imgTestArray", ifstream::in | ifstream::binary);
            testlabl.open("imgTestLabel", ifstream::in | ifstream::binary);
            if ((!testStream) || !testlabl)
            {
                if (testStream)
                    testStream.close();
                if (testlabl)
                    testlabl.close();
                cout << "There does not exist test data" << endl;
                return -1.f;
            }

            while (EOF != testStream.peek())
            {
                count++;

                testStream.read(inRaw, sizeof(char)* inUnitNum);
                testlabl.read(&tlabl, sizeof(char));

                RegularMap(inRaw, inMap);

                ForwardC1();
                ForwardS1();
                ForwardC2();
                ForwardS2();
                ForwardC3();
                ForwardC4();
                ForwardC5();
                ForwardS5();
                ForwardF1();
                ForwardF2();
                ForwardF3();

                for (mIdx = 0; mIdx < f3UnitNum; mIdx++)
                {
                    maxv = f3Map[mIdx] > maxv ? f3Map[mIdx] : maxv;
                    maxI = f3Map[mIdx] > maxv ? mIdx : maxI;
                }

                if (maxI == tlabl)
                    prc += 1;
            }

            testStream.close();
            testlabl.close();

            return (prc / count);
        }



        void slqAlexNet::SaveParameters()
        {
            ofstream fileStream;

            fileStream.open("alexParams", ofstream::out | ofstream::binary);
            if (!fileStream)
            {
                cout << "Error to create Parameters file" << endl;
                return;
            }

            fileStream.write((char*)c1Conv, c1ConvUNum * sizeof(float));
            fileStream.write((char*)c1BiasDt, c1MapNum * sizeof(float));

            fileStream.write((char*)s1PoolDt, s1MapNum * sizeof(float));
            fileStream.write((char*)s1BiasDt, s1MapNum * sizeof(float));

            fileStream.write((char*)c2ConvDt, c2ConvUNum * sizeof(float));
            fileStream.write((char*)c2BiasDt, c2MapNum * sizeof(float));

            fileStream.write((char*)s2PoolDt, s2MapNum * sizeof(float));
            fileStream.write((char*)s2BiasDt, s2MapNum * sizeof(float));

            fileStream.write((char*)c3ConvDt, c3ConvUNum * sizeof(float));
            fileStream.write((char*)c3BiasDt, c3MapNum * sizeof(float));

            fileStream.write((char*)c4ConvDt, c4ConvUNum * sizeof(float));
            fileStream.write((char*)c4BiasDt, c4MapNum * sizeof(float));

            fileStream.write((char*)c5ConvDt, c5ConvUNum * sizeof(float));
            fileStream.write((char*)c5BiasDt, c5MapNum * sizeof(float));

            fileStream.write((char*)s5PoolDt, s5MapNum * sizeof(float));
            fileStream.write((char*)s5BiasDt, s5MapNum * sizeof(float));

            fileStream.write((char*)f1ConnDt, f1ConnNum * sizeof(float));
            fileStream.write((char*)f1BiasDt, f1UnitNum * sizeof(float));

            fileStream.write((char*)f2ConnDt, f2ConnNum * sizeof(float));
            fileStream.write((char*)f2BiasDt, f2UnitNum * sizeof(float));

            fileStream.write((char*)f3ConnDt, f3ConnNum * sizeof(float));
            fileStream.write((char*)f3BiasDt, f3UnitNum * sizeof(float));

        }


        void slqAlexNet::ReadParameters()
        {
            ifstream fileStream;

            fileStream.open("alexParams", ifstream::in | ifstream::binary);
            if (!fileStream)
            {
                cout << "Error to Open Params file" << endl;
                return;
            }

            fileStream.read((char*)c1Conv, c1ConvUNum * sizeof(float));
            fileStream.read((char*)c1BiasDt, c1MapNum * sizeof(float));

            fileStream.read((char*)s1PoolDt, s1MapNum * sizeof(float));
            fileStream.read((char*)s1BiasDt, s1MapNum * sizeof(float));

            fileStream.read((char*)c2ConvDt, c2ConvUNum * sizeof(float));
            fileStream.read((char*)c2BiasDt, c2MapNum * sizeof(float));

            fileStream.read((char*)s2PoolDt, s2MapNum * sizeof(float));
            fileStream.read((char*)s2BiasDt, s2MapNum * sizeof(float));

            fileStream.read((char*)c3ConvDt, c3ConvUNum * sizeof(float));
            fileStream.read((char*)c3BiasDt, c3MapNum * sizeof(float));

            fileStream.read((char*)c4ConvDt, c4ConvUNum * sizeof(float));
            fileStream.read((char*)c4BiasDt, c4MapNum * sizeof(float));

            fileStream.read((char*)c5ConvDt, c5ConvUNum * sizeof(float));
            fileStream.read((char*)c5BiasDt, c5MapNum * sizeof(float));

            fileStream.read((char*)s5PoolDt, s5MapNum * sizeof(float));
            fileStream.read((char*)s5BiasDt, s5MapNum * sizeof(float));

            fileStream.read((char*)f1ConnDt, f1ConnNum * sizeof(float));
            fileStream.read((char*)f1BiasDt, f1UnitNum * sizeof(float));

            fileStream.read((char*)f2ConnDt, f2ConnNum * sizeof(float));
            fileStream.read((char*)f2BiasDt, f2UnitNum * sizeof(float));

            fileStream.read((char*)f3ConnDt, f3ConnNum * sizeof(float));
            fileStream.read((char*)f3BiasDt, f3UnitNum * sizeof(float));

        }


        void slqAlexNet::RandomBias(float *randVector, int vLen)
        {}


        void slqAlexNet::ProduceLabel()
        {
            int lIdx;
            for (lIdx = 0; lIdx < f3UnitNum; lIdx++)
            {
                if (curLabl == lIdx)
                {
                    mlabel[lIdx] = 0.8f;
                }
                else
                {
                    mlabel[lIdx] = -0.8f;
                }
            }
        }


        void slqAlexNet::RegularMap(char *cmap, float *mapdata)
        {
            int mIdx;
            float maxv = -10.f;
            float minv = 256.f;
            char *curc = cmap;
            float *curmap = mapdata;

            for (mIdx = 0; mIdx < inMapSize; mIdx++)
            {
                float cur = curc[mIdx] < 0 ? (curc[mIdx] + 256.f) : curc[mIdx];
                maxv = cur > maxv ? cur : maxv;
                minv = cur < minv ? cur : minv;
            }

            for (mIdx = 0; mIdx < inMapSize; mIdx++)
            {
                float cur = curc[mIdx] < 0 ? (curc[mIdx] + 256.f) : curc[mIdx];
                curmap[mIdx] = (cur - minv) / (maxv - minv) * 2.f - 1.f;
            }

            maxv = -10.f;
            minv = 256.f;
            curc = cmap + inMapSize;
            curmap = mapdata + inMapSize;
            for (mIdx = 0; mIdx < inMapSize; mIdx++)
            {
                float cur = curc[mIdx] < 0 ? (curc[mIdx] + 256.f) : curc[mIdx];
                maxv = cur > maxv ? cur : maxv;
                minv = cur < minv ? cur : minv;
            }

            for (mIdx = 0; mIdx < inMapSize; mIdx++)
            {
                float cur = curc[mIdx] < 0 ? (curc[mIdx] + 256.f) : curc[mIdx];
                curmap[mIdx] = (cur - minv) / (maxv - minv) * 2.f - 1.f;
            }

            maxv = -10.f;
            minv = 256.f;
            curc = cmap + inMapSize*2;
            curmap = mapdata + inMapSize*2;
            for (mIdx = 0; mIdx < inMapSize; mIdx++)
            {
                float cur = curc[mIdx] < 0 ? (curc[mIdx] + 256.f) : curc[mIdx];
                maxv = cur > maxv ? cur : maxv;
                minv = cur < minv ? cur : minv;
            }

            for (mIdx = 0; mIdx < inMapSize; mIdx++)
            {
                float cur = curc[mIdx] < 0 ? (curc[mIdx] + 256.f) : curc[mIdx];
                curmap[mIdx] = (cur - minv) / (maxv - minv) * 2.f - 1.f;
            }

        }


        void slqAlexNet::uniform_rand(float* src, int len, float min, float max)
        {
            int rIdx = 0;

            for (; rIdx < len; rIdx++)
            {
                src[rIdx] = uniform_rand(min, max);
            }
        }


        float slqAlexNet::uniform_rand(float min, float max)
        {
            static float U, V;
            static int phase = 0;
            float gaussz = 0;

            if (0 == phase)
            {
                U = (rand() + EspCNN) / (RAND_MAX + 1.0f);
                V = (rand() + EspCNN) / (RAND_MAX + 1.0f);

                gaussz = 0.01f * std::sqrt(-2.0f * log(U)) * sin(2.0f * CV_PI * V);
            }
            else
            {
                gaussz = 0.01f * std::sqrt(-2.0f * log(U)) * cos(2.0f * CV_PI * V);
            }

            phase = 1 - phase;
            return gaussz;
        }



    } // end namespace slqAlexNet

} // end namespace slqDL
