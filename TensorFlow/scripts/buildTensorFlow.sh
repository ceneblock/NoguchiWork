#!/bin/bash

#Create and activate the environment
python3 -m venv tensorFlow
source tensorFlow/bin/activate

#Setup some stuff for jupyter notebook
python -m pip install --upgrade pip
pip install ipykernel
python -m ipykernel install --user --name=tensorFlow

#Setup prereqs
pip install opencv-python
pip install --upgrade pyqt5 lxml

###                     NOTE:
###THESE NEXT FEW STEPS MAY FAIL! THAT IS OKAY IF YOU ALREADY HAVE THE
###                   REPOS CLONED

#LabelImg
if [ -d labelImg ]
then
  pushd ./labelImg
  git pull
  popd
else
  git clone https://github.com/tzutalin/labelImg
fi

#TensorFlow
if [ -d models ]
then
  pushd ./models
  git pull
  popd
else
  git clone https://github.com/tensorflow/models
fi

###                     NOTE:
###THOSE PREVIOUS FEW STEPS MAY FAIL! THAT IS OKAY IF YOU ALREADY HAVE THE
###                   REPOS CLONED

#Get protobuf
if [ ! -d protobuf-3.17.3 ]
then
  wget https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.17.3.tar.gz
  tar -xf v3.17.3.tar.gz
  cd ./protobuf-3.17.3
  ./autogen.sh
  ./configure
  make -j 16
  sudo make install
  sudo ldconfig
fi

#Go build labelImg
pushd labelImg
make qt5py3
popd

pushd ./models/research && protoc object_detection/protos/*.proto --python_out=. && cp object_detection/packages/tf2/setup.py . && python -m pip install .
python ./object_detection/builders/model_builder_tf2_test.py
popd

