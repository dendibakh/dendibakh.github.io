---
layout: post
title: Book Updates and Errata. Performance Analysis and Tuning on Modern CPUs (Second Edition)
categories: [personal]
---

------

**Subscribe to my [newsletter](https://products.easyperf.net/newsletter), and support me on [Patreon](https://www.patreon.com/dendibakh), [Github](https://github.com/sponsors/dendibakh), or by PayPal [donation](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBM3NW8TKTT34&currency_code=USD&source=url).**

------

I will use this page to provide updates and errata for the second edition of my book "Performance Analysis and Tuning on Modern CPUs".

### Updates and General Information 

11-Nov-2024: The book is now available on [Amazon](https://amzn.to/4fsroBs).

22-Nov-2024: [Hardcover](https://amzn.to/3ZGNgmQ) and [Kindle](https://amzn.to/3VrD3Zc) versions are available.

03-Dec-2024: The PDF version is available for downloading from [GitHub](https://github.com/dendibakh/perf-book/releases/tag/2.0_release).

------

### Errata

See a list of opened [Github Issues](https://github.com/dendibakh/perf-book/issues).

22-Nov-2024: A couple of readers of the paperback version have reported that there are some blurry pages and some pages have purple-ish text color (instead of black). I acknowledge this issue and I'm trying to fix it. The hardcover version (with premium color printing) seems not to have this problem. It turns out to be an issue with the LaTeX to PDF conversion. Some details are here: https://www.kdpcommunity.com/s/question/0D7at0000022jCnCAI.

24-Dec-2024: The following link on page 253 (Chapter 11, PGO) is outdated: https://github.com/google/llvm-propeller/blob/plo-dev/Propeller_RFC.pdf, use the following link instead: https://dl.acm.org/doi/abs/10.1145/3575693.3575727.

24-Dec-2024: Bad image formatting in Appendix: some images cover the text.

24-Dec-2024: The following link on page 332 (Appendix C, Intel PT) is outdated: https://sites.google.com/site/intelptmicrotutorial/.

20-May-2025: The following link in the footnote on page 252 ("HFSort in LLD") is broken: https://github.com/llvm-project/lld/blob/master/ELF/CallGraphSort.cpp. Here is the correct link: https://github.com/llvm/llvm-project/blob/main/lld/ELF/CallGraphSort.cpp.

02-Jun-2025: Typo in Chapter 4 Section 10. ([Link](https://github.com/dendibakh/perf-book/commit/389a8a22ecd4e51291e3b3d71396d1928deabc13))

06-Jun-2025: Figure 3.13 is incorrect; the bits in the figure sum up to 73 (16 + 9 + 9 + 9 + 9 + 21) while they should sum up to 64.

| [![](/img/posts/book_errata/HugePageVirtualAddress_old.png){: .center-image-width-100 }]({{ site.url }}/img/posts/book_errata/HugePageVirtualAddress_old.png) | 
|:--:| 
| *OLD: Figure 3.13: Virtual address that points within a 2MB page.* |

| [![](/img/posts/book_errata/HugePageVirtualAddress_new.png){: .center-image-width-100 }]({{ site.url }}/img/posts/book_errata/HugePageVirtualAddress_new.png) | 
|:--:| 
| *NEW: Figure 3.13: Virtual address that points within a 2MB page.* |

09-Jun-2025: Error in Section 5.5 "The Roofline Performance Model": peak memory bandwidth should be in GB, not in GiB.