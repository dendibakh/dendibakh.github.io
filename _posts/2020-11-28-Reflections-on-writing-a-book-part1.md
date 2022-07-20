---
layout: post
title: Reflections on Writing a Book. Part 1.
categories: [personal]
---

------
**Subscribe to my [newsletter](https://mailchi.mp/4eb73720aafe/easyperf), support me on [Patreon](https://www.patreon.com/dendibakh) or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

In my [previous post]({{ site.url }}/blog/2020/11/22/Writing-A-Free-Book-From-The-Start), I shared my thoughts on why I decided to make my [book](https://book.easyperf.net/perf_book) free. I will share some interesting numbers in today's post, how I managed reviews and contributions. This is a random collection of thoughts and reflections, what worked well and what I would do differently.

I wrote the entire book in [Markdown](https://en.wikipedia.org/wiki/Markdown), which is a plain-text format and then used [pandoc](https://pandoc.org/) to convert it to [LaTeX](https://www.latex-project.org/). Then, a tool called `pdflatex` can generate PDF out of raw LaTeX input. I also used `git` to keep track of the progress. Those tools are free, and there are plenty of good Markdown editors out there; I used [Typora](https://typora.io/). I think writing a book in a plain text format like Markdown is super convenient: you can easily `diff` your changes. Also, Markdown plus pandoc tooling allows you to have a clean setup. You can separate images from the text and split the book into small text files.

I think `MarkDown` plus `pandoc` is a pretty popular setup for writing a book. However, there is at least one big downside that I faced. I spent a significant amount of time trying to achieve the desired formatting. Yes, you can google many things pretty quickly. But if you are new to LaTeX, you can have a hard time to format all the figures, tables, and listings properly. There is a lot of small formatting details you should care about: code highlighting, links, footnotes, section references, book citations, glossary, table of contents, and so on and so on. For example, I spent ~3 hours trying to render tables with cell borders with `pandoc` and still haven't found a way to do this. Anyway, I hope you won't judge my book by the formatting. Pandoc and LaTeX are not my biggest strength.

I spent 600-700 hours writing — a massive amount of time but not an extraordinary effort. I know people that spent more than 3000 hours on their books. From that perspective, this number doesn't seem so big but taking into account that I have a full-time job. Well, let's say it wasn't easy. I think only 75% of the time I spent on writing the book. Managing reviews and contributions took a significant amount of time, I guess around %20. Another 5% of the time went into configuring the environment, formatting, etc. The process of writing itself is the longest and most demanding, no surprise. It takes a lot of discipline and dedication to write the complete book, but I will expand more on this in the [next post]({{ site.url }}/blog/2020/11/29/Reflections-on-writing-a-book-part2).

I decided to conduct three rounds of review. This wasn't a number I set from the beginning. I started a new round of review whenever I felt a need to do so or after I rewrote some chapters. I hosted online reviews with the help of the tool named [hypothesis](https://web.hypothes.is/). All it takes from you is just to upload the PDF somewhere on the web and provide a link to it. The interface is pretty intuitive and straightforward, you can post your comments online, and everyone else will see them. Hypothesis lacks some "nice-to-have" features[^1], but it is much better than offline review when people embed comments into the pdf itself.

More than 150 people agreed to review my book. Of those 150 people, 40-50 started the review, and only 13-15 read the whole book (170 pages). That is an acceptable conversion rate, and it made a pretty good coverage and quality of the review. I tried to form packs of people with different skills and sometimes even asked to review from a particular angle. Many times I asked experts in a specific area to review a particular chapter, and so on. The biggest issue in reviews was that people usually didn't have much time, considering this was not a paid job. Anyway, I'm enormously grateful to all the reviewers of my book. Without their help, the book wouldn't come out of the draft.

In total, there were 723 review comments. That's a lot of bugs. On average, it is more than four comments per page. Half of them were just a typo or a small issue that I could fix "on-the-fly". Others were not that simple. There is one chapter in the book that I rewrote two times almost from scratch. It was not because I was utterly ignorant of that particular topic. It was just because I didn't have a bigger picture and a sense of what problems people are facing. It took me a lot of time to address all the comments; however, I still get emails from people pointing out some inaccurate statements in the book even after release. So, stay tuned. There will be a second edition in a few months.

Speaking of a bigger picture, the whole review process was very educational to me. You see, those 50 people that reviewed my book shared their experience and knowledge, which not only made the book better but changed my perspective on certain things. It's hard to look outside the box when you work in the same environment for many years. We had many great conversations with engineers representing industries such as HFT, HPC, game dev, academia, and others.

Allright, that's all that I have for today. In my [next post]({{ site.url }}/blog/2020/11/29/Reflections-on-writing-a-book-part2), I write about what I think is the most important skill you need to write a book.

---

[^1]: For example, you cannot mark a comment as addressed.
