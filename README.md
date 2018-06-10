# Track-eye-iris-and-draw-the-movements-accordingly

Goal:-
	To track the movements of the iris and draw on the screen accordingly.

Abstract:-
	Our project , predominantly is based on iris tracking for which eye localisation is achieved first using the algorithm viola-jones and then the next step is to detect the iris using Hough Circles in which the Edge Analysis algorithm is used.We then draw the movements of eye accordingly.We also calculate the angle at which the user is looking with respect to the camera and plot the rate of change of these angles with time. 
 
Introduction:-
        In this, we do  video processing which means we take each frame from the video as a input to begin our work.	
	The first step is to locate the face from the frame using viola-jones. viola-jones is a technique to detect objects using haar features.The picture of haar masks are shown at the end of the report.Those masks are slided over the image, and the sum of the values of the pixels within the “white” sides is subtracted from the “black” sides. Now the result is a feature that represents that region.
WEAK CLASSIFIER:-
	The second is step is to train many simple classifiers. Those simple classifiers work as follows: 
It takes all the features within the face region and all the features outside the face region, and label them as “face” or “non-face” . It then learns to distinguish features belonging to a face region from features belonging to a non-face region through a simple threshold function (i.e., faces features generally have value above or below a certain value, otherwise it’s a non-face). This classifier itself is very bad and is almost as good as random guessing. But if combined, they can arise a much better and stronger classifier .
CASCADING CLASSIFIER:-
	 Each weak classifier will output a number, 1 if it predicted the region as belonging to a face region or 0 otherwise. This result can be weighted. The sum of all weak classifiers weighted outputted results in another feature, that, again, can be inputted to another classifier. It’s said that that new classifier is a linear combination of other classifiers. Its role is to determine the right weight values such as the error be as minimum as possible.
	Eyes follow the same principle as face detection. But now, if we have a face detector previously trained, the problem becomes sightly simpler, since the eyes will be always located in the face region, reducing dramatically our search space.
DETECTING IRIS:-
	Now we have detected the eyes, the next step is to detect the iris. For that, we are going to look for the most “circular” object in the eye region. It’s called HoughCircles, and it works as follows: It first apply an edge detector in the image, from which it make contours and from the contours made it tried to calculate a “circularity ratio”, i.e., how much that contour looks like a circle.
	As the function itself says, it can detect many circles, but we just want one. So let’s select the one belonging to the eyeball. For that, we have a used a  heuristic: Choose the circle that contains more “black” pixels in it! In another words, the circle from which the sum of pixels within it is minimal.
	In order to know if a pixel is inside a pixel or not, we just test if the euclidean distance between the pixel location and the circle center is not higher than the circle radius.
	However, the HoughCircles algorithms is very unstable, and therefore the iris location can vary a lot! We need to stabilize it to get better results. To do that, we simply calculate the mean of the last five detected iris locations.
	The hough circles algorithms are :-
	-> Cumulative Distribution Function (CDF) Algorithm
	-> General Projection Functions (PF) Algorithm
	-> Edge analysis (EA)
In this project we are using EA algorithm  to track the iris. 

Iris-movements & Drawing lines:-
	We detect the iris continuously and make the mouse pointer accordingly. Using these movements of the mouse we draw them on a plain window thereby making the iris draw the lines.
	
Angle calculation and Plotting:-
	Taking a reference point when looking at the camera, we calculate the angle at which we are looking with respect to this point. We also plot a graph which indicates the rate of change of angle movements with time.
