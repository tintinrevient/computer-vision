# Action 

## How to load models
```
from keras import models

model1 = models.load_model('./models/frames_stanford40.h5')
model2 = models.load_model('./models/frames_tvhi.h5')
model3 = models.load_model('./models/optical_flow_tvhi.h5')
model4 = models.load_model('./models/two_stream_tvhi.h5')
```

## How to load weights
```
import numpy as np

weights1 = np.load('./weights/frames_stanford40.npy', allow_pickle=True)
weights2 = np.load('./weights/frames_tvhi.npy', allow_pickle=True)
weights3 = np.load('./weights/optical_flow_tvhi.npy', allow_pickle=True)
weights4 = np.load('./weights/two_stream_tvhi.npy', allow_pickle=True)
```

## References
* https://keras.io/preprocessing/sequence/
* https://www.tensorflow.org/api_docs/python/tf/keras/preprocessing/sequence/TimeseriesGenerator
* https://keras.io/layers/wrappers/
* https://www.tensorflow.org/api_docs/python/tf/keras/layers/TimeDistributed
* https://keras.io/examples/cifar10_resnet/
* https://keras.io/callbacks/