x <- read.table("points.txt")
recall <- c(0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1)*100
pure_vector <- x[1:11,1]*100
pure_vector
vector_anchor <- x[12:22,1]*100
vector_anchor
vector_page_rank <- x[23:33,1]*100
vector_page_rank
jpeg("curves.jpg")
plot(recall, pure_vector, ylim=range(c(0,100)), type = "o", col="blue", xlab="Recall", ylab="Precision")
par(new = TRUE)
plot(recall, vector_anchor, ylim=range(c(0,100)), type = "o", col="red", axes = FALSE, xlab = "", ylab = "")
par(new = TRUE)
plot(recall, vector_page_rank, ylim=range(c(0,100)), type = "o", col="purple", axes = FALSE, xlab = "", ylab = "")
title(main="Precision X Recall: query")
legend(59.5, 103.5, c("Vetorial", "Vetorial + Anchor", "Vetorial + PageRank"), lty=c(1,1), lwd=c(2.5, 2.5), col=c("blue", "red", "purple"))
dev.off()
