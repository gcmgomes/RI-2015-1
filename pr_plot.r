x <- read.table("points.txt")
q <- x[, 1]
p_5 <- x[, 2]*100
p_5
p_10 <- x[, 3]*100
p_10
jpeg("curves.jpg")
plot(q, p_5, ylim=range(c(0,100)), type = "o", col="blue", xlab="q factor", ylab="Precision")
par(new = TRUE)
plot(q, p_10, ylim=range(c(0,100)), type = "o", col="red", axes = FALSE, xlab = "", ylab = "")
par(new = TRUE)
title(main="Precision X PageRank q factor")
legend(0.81, 103.5, c("P@5", "P@10"), lty=c(1,1), lwd=c(2.5, 2.5), col=c("blue", "red"))
dev.off()
