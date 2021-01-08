import matplotlib

from keras.models import Sequential
from keras.layers import Conv2D
import numpy as np
import os
from keras.preprocessing.image import load_img, img_to_array

# change working directory to script path
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

noisy_imgs_path = 'training_data/128_gray/smooth/'
clean_imgs_path = 'training_data/128_gray/jaggy/'

dim = 128

X_train_noisy = []
X_train_clean = []

for file in sorted(os.listdir(noisy_imgs_path)):
    img = load_img(noisy_imgs_path + file,
                   color_mode='grayscale', target_size=(dim, dim))
    img = img_to_array(img).astype('float32') / 255
    X_train_noisy.append(img)

for file in sorted(os.listdir(clean_imgs_path)):
    img = load_img(clean_imgs_path + file,
                   color_mode='grayscale', target_size=(dim, dim))
    img = img_to_array(img).astype('float32') / 255
    X_train_clean.append(img)

# convert to numpy array
X_train_noisy = np.array(X_train_noisy)
X_train_clean = np.array(X_train_clean)

amount_test = 10
# use the first "amount_test" noisy images as testing images
X_test_noisy = X_train_noisy[0:amount_test, ]
X_train_noisy = X_train_noisy[amount_test + 1:, ]

# use the first "amount_test" clean images as testing images
X_test_clean = X_train_clean[0:amount_test, ]
X_train_clean = X_train_clean[amount_test + 1:, ]

# Build and train model
conv_autoencoder = Sequential()
conv_autoencoder.add(Conv2D(filters=32, kernel_size=(
    3, 3), input_shape=(dim, dim, 1), activation='relu', padding='same'))
conv_autoencoder.add(Conv2D(filters=16, kernel_size=(
    3, 3), activation='relu', padding='same'))
conv_autoencoder.add(Conv2D(filters=8, kernel_size=(3, 3),
                            activation='relu', padding='same'))
conv_autoencoder.add(Conv2D(filters=8, kernel_size=(3, 3),
                            activation='relu', padding='same'))
conv_autoencoder.add(Conv2D(filters=16, kernel_size=(
    3, 3), activation='relu', padding='same'))
conv_autoencoder.add(Conv2D(filters=32, kernel_size=(
    3, 3), activation='relu', padding='same'))
conv_autoencoder.add(Conv2D(filters=1, kernel_size=(3, 3),
                            activation='sigmoid', padding='same'))

conv_autoencoder.compile(optimizer='adam', loss='binary_crossentropy')
conv_autoencoder.fit(X_train_noisy, X_train_clean, epochs=2)


output = conv_autoencoder.predict(X_test_noisy)

out_folder = "out_images"
if not os.path.exists(out_folder):
    os.makedirs(out_folder)

for i in range(amount_test):
    im = np.array((output[i]).reshape((dim, dim)) * 255).astype(int)
    matplotlib.image.imsave(out_folder + "/_" + str(i) +
                            "_generated.png", im, cmap='gray')

    im = np.array((X_test_clean[i]).reshape((dim, dim)) * 255).astype(int)
    matplotlib.image.imsave(out_folder + "/_" + str(i) +
                            "_clean.png", im, cmap='gray')

    im = np.array((X_test_noisy[i]).reshape((dim, dim)) * 255).astype(int)
    matplotlib.image.imsave(out_folder + "/_" + str(i) +
                            "_noisy.png", im, cmap='gray')
