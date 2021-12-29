#!/bin/env python

import os
import wget
import tarfile
import zipfile

BASE_DIRECTORY=""
try:
  from google.colab import drive
  drive.mount('/content/drive')
  BASE_DIRECTORY=os.path.join(os.getcwd(),'drive','MyDrive','TensorFlow')
except ImportError:
  os.makedirs("content", exist_ok=True)
  BASE_DIRECTORY="/storage1/noguchik/Active/cellquant/TensorFlow"
    
# Set up some globals
try:
    os.environ["LD_LIBRARY_PATH"] = os.environ["LD_LIBRARY_PATH"] + ":/usr/local/cuda/lib64"
except KeyError:
    os.environ["LD_LIBRARY_PATH"] = "/usr/local/cuda/lib64"

UNIQUE_MODIFIER='2021-11-9'
CUSTOM_MODEL_NAME = 'my_centernet' + '_' + UNIQUE_MODIFIER
PRETRAINED_MODEL_NAME = 'centernet_hg104_512x512_kpts_coco17_tpu-32'
PRETRAINED_MODEL_URL = 'http://download.tensorflow.org/models/object_detection/tf2/20200711/centernet_hg104_512x512_kpts_coco17_tpu-32.tar.gz'
TF_RECORD_SCRIPT_NAME = 'generate_tfrecord.py'
LABEL_MAP_NAME = 'label_map.pbtxt'

paths = {
    'WORKSPACE_PATH': os.path.join(BASE_DIRECTORY, 'workspace'),
    
    'ANNOTATION_PATH': os.path.join(BASE_DIRECTORY, 'workspace','annotations', UNIQUE_MODIFIER),
    
    'MODEL_PATH' : os.path.join(BASE_DIRECTORY, 'workspace','models'),
    'CHECKPOINT_PATH': os.path.join(BASE_DIRECTORY, 'workspace','models',CUSTOM_MODEL_NAME), 
    'OUTPUT_PATH': os.path.join(BASE_DIRECTORY, 'workspace','models',CUSTOM_MODEL_NAME, 'export'), 
    'TFJS_PATH':os.path.join(BASE_DIRECTORY, 'workspace','models',CUSTOM_MODEL_NAME, 'tfjsexport'), 
    'TFLITE_PATH':os.path.join(BASE_DIRECTORY, 'workspace','models',CUSTOM_MODEL_NAME, 'tfliteexport'), 
    
    'IMAGE_PATH': os.path.join(BASE_DIRECTORY, 'workspace','images', UNIQUE_MODIFIER),
    'PRETRAINED_MODEL_PATH': os.path.join(BASE_DIRECTORY, 'workspace','pre-trained-models'),
    
    
    'SCRIPTS_PATH': os.path.join(BASE_DIRECTORY,'scripts'),

    'PROTOC_PATH':os.path.join(BASE_DIRECTORY,'protoc'),
    

 }

files = {
    'PIPELINE_CONFIG':os.path.join(paths['WORKSPACE_PATH'],'models', CUSTOM_MODEL_NAME, 'pipeline.config'),
    'TF_RECORD_SCRIPT': os.path.join(paths['SCRIPTS_PATH'], TF_RECORD_SCRIPT_NAME), 
    'LABELMAP': os.path.join(paths['ANNOTATION_PATH'], LABEL_MAP_NAME)
}
paths['TRAIN_IMAGES'] = os.path.join(paths['IMAGE_PATH'], 'train')
paths['TEST_IMAGES'] = os.path.join(paths['IMAGE_PATH'], 'test')

# Make sure all the directories exist
for path in paths.values():
  os.makedirs(path, exist_ok=True)

#Change Directories
os.chdir(BASE_DIRECTORY)
print(os.getcwd())

#See if we need to grab the repos (we shouldn't)
if not os.path.isdir(os.path.join(BASE_DIRECTORY, 'models')):
  try:
    from git import Repo
    Repo.clone_from('https://github.com/tensorflow/models.git', 'models')
  except ImportError:
    os.system("git clone https://github.com/tensorflow/models.git")
    
#Update the paths array

paths['APIMODEL_PATH'] = os.path.join(BASE_DIRECTORY,'models')

TRAINING_SCRIPT = os.path.join(paths['APIMODEL_PATH'], 'research', 'object_detection', 'model_main_tf2.py')

os.system("pwd")

#TensorFlow API Installation
if os.name=='posix':
    #This was is better since it's archicture independent, but it requires knowing if the OS is Debian based
    #!apt-get install protobuf-compiler
    url="https://github.com/protocolbuffers/protobuf/releases/download/v3.18.0/protoc-3.18.0-linux-x86_64.zip"
    file_name = wget.download(url)
    os.rename(file_name,os.path.join(paths['PROTOC_PATH'],file_name))
    os.chdir(paths['PROTOC_PATH'])

    with zipfile.ZipFile(os.path.join(paths['PROTOC_PATH'],file_name), "r") as zip_ref:
        zip_ref.extractall(paths['PROTOC_PATH'])
    os.system("chmod +x" +  paths['PROTOC_PATH'] + "/bin/*");
    
    os.environ['PATH'] += os.pathsep + os.path.abspath(os.path.join(paths['PROTOC_PATH'], 'bin'))  
    os.system("cd " + BASE_DIRECTORY + "/models/research && protoc object_detection/protos/*.proto --python_out=. && cp object_detection/packages/tf2/setup.py . && python -m pip install .")
    
"""
if os.name=='nt':
    url="https://github.com/protocolbuffers/protobuf/releases/download/v3.15.6/protoc-3.15.6-win64.zip"
    wget.download(url)
    !move protoc-3.15.6-win64.zip {paths['PROTOC_PATH']}
    !cd {paths['PROTOC_PATH']} && tar -xf protoc-3.15.6-win64.zip
    os.environ['PATH'] += os.pathsep + os.path.abspath(os.path.join(paths['PROTOC_PATH'], 'bin'))   
    !cd {BASE_DIRECTORY}/models/research && protoc object_detection/protos/*.proto --python_out=. && copy object_detection\\packages\\tf2\\setup.py setup.py && python setup.py build && python setup.py install
    !cd {BASE_DIRECTORY}/models/research/slim && pip install -e .
"""
VERIFICATION_SCRIPT = os.path.join(paths['APIMODEL_PATH'], 'research', 'object_detection', 'builders', 'model_builder_tf2_test.py')
os.system("python " + VERIFICATION_SCRIPT)

