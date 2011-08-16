/*********************************************************************
MLDemos: A User-Friendly visualization toolkit for machine learning
Copyright (C) 2010  Basilio Noris
Contact: mldemos@b4silio.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License,
version 3 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*********************************************************************/
#include "pluginHMM.h"
#include "drawUtils.h"
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QDebug>

using namespace std;

PluginHMM::PluginHMM()
{
	params = new Ui::ParametersHMM();
	params->setupUi(widget = new QWidget());
}

void PluginHMM::SetParams(Dynamical *dynamical)
{
	if(!dynamical) return;

	int mixtures = params->hmmMixtureCount->value();
	int states = params->hmmStatesCount->value();
	int trainType = params->hmmTrainCombo->currentIndex();
	int obsType = params->hmmObsCombo->currentIndex();

	((DynamicalHMM *)dynamical)->SetParams(mixtures, states, trainType, obsType);
}

Dynamical *PluginHMM::GetDynamical()
{
	DynamicalHMM *dynamical = new DynamicalHMM();
	SetParams(dynamical);
	return dynamical;
}

void PluginHMM::DrawInfo(Canvas *canvas, QPainter &painter, Dynamical *dynamical)
{
	if(!canvas || !dynamical) return;
	painter.setRenderHint(QPainter::Antialiasing);

	DynamicalHMM *hmm = (DynamicalHMM*)dynamical;
	CVectorObsProb *b = (CVectorObsProb*) hmm->b;
	int dim = b->GetDimension();
	CObsProb **comp = b->GetComponents();
	int states = b->GetN();
	fvec mean(dim);
	fvec sigmas(dim);
	fvec sigma(3);
	FOR(i, states)
	{
		if(hmm->IsOrphanedState(i)) continue;
		FOR(d,dim)
		{
			CGaussianObsProb *g = (CGaussianObsProb*)comp[d+1];
			double v = g->GetMean()[i+1];
			v /= 1000.;
			mean[d] = v;
			double s = g->GetStd()[i+1];
			s /= 1000.;
			sigmas[d] = s;
		}
		sigma[0] = sigmas[0];
		sigma[2] = sigmas[1];

		painter.setPen(QPen(Qt::black, 2));
		DrawEllipse(&mean[0], &sigma[0], 0.5, &painter, canvas);
		painter.setPen(QPen(Qt::black, 1));
		DrawEllipse(&mean[0], &sigma[0], 1, &painter, canvas);

		QPointF point = canvas->toCanvasCoords(mean);
		QColor color = SampleColor[(i+1)%SampleColorCnt];
		painter.setPen(QPen(Qt::black, 12));
		painter.drawEllipse(point, 8, 8);
		painter.setPen(QPen(color,4));
		painter.drawEllipse(point, 8, 8);
	}

	std::vector< std::vector<fvec> > trajectories = hmm->trajectories;
	FOR(i, trajectories.size())
	{
		int count = trajectories[i].size();
		if(!count) continue;
		ivec stateSequence(count+1);

		CVectorObs** observations = new CVectorObs*[count+1];
		FOR(j, count+1) observations[j] = new CVectorObs(dim);
		for(int j=0;j<count;j++){
			FOR(d,dim) observations[j+1]->SetDouble(trajectories[i][j][d],d+1);
		}

		hmm->learnedHMM->ViterbiLog((CObs**)observations, count, &stateSequence[0]);
		std::cout << "states:";
		FOR(j, count)
		{
			std::cout << " " << stateSequence[j+1];
			fvec sample = trajectories[i][j];
			FOR(d, dim) sample[d] /= 1000.f;
			QPointF point = canvas->toCanvasCoords(sample);
			Canvas::drawSample(painter, point, 8, stateSequence[j+1]);
		}
		std::cout << std::endl;
		FOR(j, count+1) delete observations[j];
		delete [] observations;
	}

	/*
	int genStep = 20;
	FOR(j,genStep)
	{
		int steps = 100;
		CObs ** obs = hmm->learnedHMM->GenerateObservations(steps);
		QPointF oldPoint;
		FOR(i, steps)
		{
			int obsDim = obs[i+1]->GetDim();
			fvec sample(obsDim);
			FOR(d, obsDim)
			{
				double v = obs[i+1]->GetDouble(d+1);
				v /= 1000.;
				sample[d] = v;
			}
			QPointF point = canvas->toCanvasCoords(sample);
			painter.setPen(QPen(SampleColor[j%SampleColorCnt], 0.5));
			if(i) painter.drawLine(point, oldPoint);
			oldPoint = point;
			//Canvas::drawSample(painter, point, 8, 2);
		}
	}
	*/

}

void PluginHMM::SaveOptions(QSettings &settings)
{
	settings.setValue("hmmMixtureCount", params->hmmMixtureCount->value());
	settings.setValue("hmmStatesCount", params->hmmStatesCount->value());
	settings.setValue("hmmTrainCombo", params->hmmTrainCombo->currentIndex());
	settings.setValue("hmmObsCombo", params->hmmObsCombo->currentIndex());
}

bool PluginHMM::LoadOptions(QSettings &settings)
{
	if(settings.contains("hmmMixtureCount")) params->hmmMixtureCount->setValue(settings.value("hmmMixtureCount").toInt());
	if(settings.contains("hmmStatesCount")) params->hmmStatesCount->setValue(settings.value("hmmStatesCount").toInt());
	if(settings.contains("hmmTrainCombo")) params->hmmTrainCombo->setCurrentIndex(settings.value("hmmTrainCombo").toInt());
	if(settings.contains("hmmObsCombo")) params->hmmObsCombo->setCurrentIndex(settings.value("hmmObsCombo").toInt());
	return true;
}

void PluginHMM::SaveParams(QTextStream &file)
{
	file << "dynamicalOptions" << ":" << "hmmMixtureCount" << " " << params->hmmMixtureCount->value() << "\n";
	file << "dynamicalOptions" << ":" << "hmmStatesCount" << " " << params->hmmStatesCount->value() << "\n";
	file << "dynamicalOptions" << ":" << "hmmTrainCombo" << " " << params->hmmTrainCombo->currentIndex() << "\n";
	file << "dynamicalOptions" << ":" << "hmmObsCombo" << " " << params->hmmObsCombo->currentIndex() << "\n";
}

bool PluginHMM::LoadParams(QString name, float value)
{
	if(name.endsWith("hmmMixtureCount")) params->hmmMixtureCount->setValue((int)value);
	if(name.endsWith("hmmStatesCount")) params->hmmStatesCount->setValue((int)value);
	if(name.endsWith("hmmTrainCombo")) params->hmmTrainCombo->setCurrentIndex((int)value);
	if(name.endsWith("hmmObsCombo")) params->hmmObsCombo->setCurrentIndex((int)value);
	return true;
}

Q_EXPORT_PLUGIN2(mld_HMM, PluginHMM)
