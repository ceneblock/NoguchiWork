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
git clone https://github.com/tzutalin/labelImg

#TensorFlow
git clone https://github.com/tensorflow/models

###                     NOTE:
###THOSE PREVIOUS FEW STEPS MAY FAIL! THAT IS OKAY IF YOU ALREADY HAVE THE
###                   REPOS CLONED

#Get protobuf
wget https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.17.3.tar.gz
tar -xf v3.17.3.tar.gz
cd ./protobuf-3.17.3
./autogen
./configure
make -j 16
sudo make install
sudo ldconfig

#Go build labelImg
cd labelImg
make qt5py3
cd ../

cd models/research && protoc object_detection/protos/*.proto --python_out=. && cp object_detection/packages/tf2/setup.py . && python -m pip install .
python ./object_detection/builders/model_builder_tf2_test.py
cd ../../

