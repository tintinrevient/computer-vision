# Keras

## Validation

### Hold-out validation
<p float="left">
	<img src="./pix/hold-out-validation.png" width="500" />
</p>

```
num_validation_samples = 10000

import numpy as np

# Shuffling the data is usually appropriate
np.random.shuffle(data)

# Define the validation set
validation_data = data[:num_validation_samples]
data = [num_validation_samples:]

# Define the training set
training_data = data[:]

# Train a model on the training data
# and evaluate it on the validation data
model = get_model()
model.train(training_data)
validation_score = model.evaluate(validation_data)

# Once you have tuned your hyperparameters,
# is it common to train your final model from scratch
# on all non-test data available.
model = get_model()
model.train(np.concatenate([training_data,
                            validation_data]))

test_score = model.evaluate(test_data)
```

### K-fold validation
<p float="left">
	<img src="./pix/k-fold-validation.png" width="700" />
</p>

```
k= 4
num_validation_samples = len(data) // k

np.random.shuffle(data)
validation_scores = []

for fold in range(k):
    # Select the validation data partition
    validation_data = data[num_validation_samples * fold: num_validation_samples * (fold + 1)]

    # The remainder of the data is used as training data.
    # Note that the "+" operator below is list concatenation, not summation
    training_data = data[:num_validation_samples * fold] + data[num_validation_samples * (fold + 1):]

    # Create a brand new instance of our model (untrained)
    model = get_model()
    model.train(training_data)

    validation_score = model.evaluate(validation_data)
    validation_scores.append(validation_score)

# This is our validation score:
# the average of the validation scores of our k folds
validation_score = np.average(validation_scores)

# We train our final model on all non-test data available
model = get_model()
model.train(data)

test_score = model.evaluate(test_data)
```

## Overfitting

* Getting more training data.
* Reducing the capacity of the network. 
* Adding weight regularization. 
* Adding dropout.

### L1 and L2 regularization

```
from keras import regularizers

model = models.Sequential()
model.add(layers.Dense(16, kernel_regularizer=regularizers.l2(0.001),
                       activation='relu', input_shape=(10000,)))
model.add(layers.Dense(16, kernel_regularizer=regularizers.l2(0.001),
                       activation='relu'))
model.add(layers.Dense(1, activation='sigmoid'))
```

```
from keras import regularizers
# L1 regularization
regularizers.l1(0.001)

# L1 and L2 regularization at the same time
regularizers.l1_l2(l1=0.001, l2=0.001)
```

### Dropout
<p float="left">
    <img src="./pix/dropout.png" width="700" />
</p>

```
# At training time:
layer_output *= np.randint(0, high=2, size=layer_output.shape)

# Note that we are scaling *up* rather scaling *down* in this case
layer_output /= 0.5
```


## References
