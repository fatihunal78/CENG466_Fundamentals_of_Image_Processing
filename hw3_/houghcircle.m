% HOUGHCIRCLE
%
% Function takes an edge map image, and constructs a Hough transfomr
% for finding circles in the image.
%
% Usage:  h = houghcircle(edgeim,rmin,rmax)
%
% arguments:
%            edgeim      - The edge map image to be transformed.
%            rmin, rmax  - The minimum and maximum radius values
%                             of circles to search for.
%
% returns:
%            h           - The Hough transform.

function h = houghcircle(edgeim, rmin, rmax)
    if(rmin > rmax)
        error('rmin CANNOT be greater than rmax!');
    end
    % Dimensions of the edge image
    [rows,cols] = size(edgeim);
    % Initialise a 3D blank accumulator array.
    acc = zeros(rows,cols,(rmax - rmin + 1));
    % Find all non-zero points in the edge image.
    [I,J] = find(edgeim > 0);
    % For each point, add a circle to the accumulator array at each level of the array (ie for every radius)
    % Display what point number is being computed out of the total number of points.
    for i = 1:length(I)
        for j = 1:(rmax - rmin + 1)
            acc(:,:,j) = addcircle(acc(:,:,j),[I(i),J(i)],rmin + j - 1);
        end
       fprintf('Point No %d out of %d \r',i,length(I));
    end
    h = acc;