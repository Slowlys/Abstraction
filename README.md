# Abstraction

## Description
This application abstracts an image taken as input.

Exemple :

![Cat][exemple]
![abstracted Cat][abstractedExemple]

The algorithm is from the research article
[Image and video abstraction using cumulative range geodesic filtering]
[link:paper]

This application can abstract grey images, color images, as well as images with transparency.

## Exploration of the parameters

Two parameters influence the resulting abstracted image : *gamma* and *maskSize*.
*maskSize* is an integer and *gamma* is a float.

To show the role of this parameters, this image will be abstracted with different values of them.

![hedgehog][hedgehog]

The default value is 100 for *maskSize* and 1.0 for *gamma* :

![abstracted hedgehog with maskSize = 100 and gamma = 1.0][hedgehog_m100]

### Influence of maskSize

With maskSize = 1 :

![abstracted hedgehog with maskSize = 1 and gamma = 1.0][hedgehog_m1]

With maskSize = 20 :

![abstracted hedgehog with maskSize = 20 and gamma = 1.0][hedgehog_m20]

With maskSize = 50 :

![abstracted hedgehog with maskSize = 50 and gamma = 1.0][hedgehog_m50]

With maskSize = 500 :

![abstracted hedgehog with maskSize = 500 and gamma = 1.0][hedgehog_m500]

### Influence of gamma

With gamma = 0.0 :

![abstracted hedgehog with maskSize = 100 and gamma = 0.0][hedgehog_g0]

With gamma = 10.0 :

![abstracted hedgehog with maskSize = 100 and gamma = 10.0][hedgehog_g10]

With gamma = 50.0 :

![abstracted hedgehog with maskSize = 100 and gamma = 50.0][hedgehog_g50]

## How to Install

First, create a directory called for exemple *build*
Then, open a terminal and execute the following commands :

```
cmake [abstraction_directory]
make
```

With *abstraction_directory* being the directory of this application, where the file *CMakeLists.txt* is present.


### Requirements

This application was only tested on Linux.

It requires the Opencv2 library.

## Usage

To abstract an image,
Please provide the path of this image with : `./Abstraction -image_path [image_path]`

You can also provide other optional arguments :
Usage :

`./Abstraction -image_path [image_path] -output_path [output_path] -maskSize [maskSize] -gamma [gamma]`



To display the usage message, you can execute the following commands :

`./Abstraction` or `./Abstraction -h` or `./Abstraction -help`

## License

This application is licensed with GNU GPLv3.


## Project Status

The developpment of this project has stopped.





[exemple]: ReadmeImages/cat.jpg "Image taken as Input"
[abstractedExemple]: ReadmeImages/abstractedCat.png "Abstracted Image"

[hedgehog]: ReadmeImages/hedgehog.jpg "Hedgehog"

[hedgehog_m1]: ReadmeImages/abstractedHedgeHog_m1.png "Abstracted hedgehog with maskSize = 1"
[hedgehog_m20]: ReadmeImages/abstractedHedgeHog_m20.png "Abstracted hedgehog with maskSize = 20"
[hedgehog_m50]: ReadmeImages/abstractedHedgeHog_m50.png "Abstracted hedgehog with maskSize = 50"
[hedgehog_m100]: ReadmeImages/abstractedHedgeHog_m100.png "Abstracted hedgehog with maskSize = 100"
[hedgehog_m500]: ReadmeImages/abstractedHedgeHog_m500.png "Abstracted hedgehog with maskSize = 500"

[hedgehog_g0]: ReadmeImages/abstractedHedgeHog_g0.png "Abstracted hedgehog with gamma = 0.0"
[hedgehog_g10]: ReadmeImages/abstractedHedgeHog_g10.png "Abstracted hedgehog with gamma = 10.0"
[hedgehog_g50]: ReadmeImages/abstractedHedgeHog_g50.png "Abstracted hedgehog with gamma = 50.0"



[link:paper]: https://www.sciencedirect.com/science/article/abs/pii/S0097849313000356#f0140 "The paper from which this application is inspired from"





