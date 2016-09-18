


# Taken from "moments" package from CRAN
# Original author at http://www.komsta.net
# These are naive implementations.

"skewness" <-
function (x, na.rm = FALSE) 
{
    if (is.matrix(x)) 
        apply(x, 2, skewness, na.rm = na.rm)
    else if (is.vector(x)) {
	if (na.rm) x <- x[!is.na(x)] 
	n <- length(x)
     (sum((x-mean(x))^3)/n)/(sum((x-mean(x))^2)/n)^(3/2)
	}
    else if (is.data.frame(x)) 
        sapply(x, skewness, na.rm = na.rm)
    else skewness(as.vector(x), na.rm = na.rm)
}

"kurtosis" <-
function (x, na.rm = FALSE) 
{
    if (is.matrix(x)) 
        apply(x, 2, kurtosis, na.rm = na.rm)
    else if (is.vector(x)) {
	if (na.rm) x <- x[!is.na(x)] 
	n <- length(x)
	n*sum( (x-mean(x))^4 )/(sum( (x-mean(x))^2 )^2)
	}
    else if (is.data.frame(x)) 
        sapply(x, kurtosis, na.rm = na.rm)
    else kurtosis(as.vector(x), na.rm = na.rm)
}

A <- commandArgs( trailingOnly=TRUE )
x <- round(runif(as.integer(A[1]),0,65535))
write( x, stdout(), ncolumns=1 )
sink( stderr() );
sprintf( "%.6e",     mean(x) );
sprintf( "%.6e",      var(x) );
sprintf( "%.6e", skewness(x) );
sprintf( "%.6e", kurtosis(x) );
sink()
