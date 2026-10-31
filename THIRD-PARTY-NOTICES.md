# Third-Party Notices

hn-reader is distributed under the MIT License (see [LICENSE](LICENSE)).
It includes or adapts code from the projects below. Those portions remain
under their original licenses.

## MarkdownFTXUI

Source: <https://github.com/zvasilev/MarkdownFTXUI>

Used in: `src/widgets/Scroll/ScrollView.hpp` (scroll frame implementation).

Copyright (c) 2026 Zhivko Vasilev

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## FTXUI

Source: <https://github.com/ArthurSonzogni/FTXUI>

Used in: `src/widgets/WrapCollapsible/WrapCollapsible.hpp` (adapted from
FTXUI's `Collapsible` component).

Copyright (c) 2019 Arthur Sonzogni
Copyright 2021 Arthur Sonzogni

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Mozilla Readability

Source: <https://github.com/mozilla/readability>

Used in: `src/model/ArticleScraper/ArticleScraper.cpp` and
`src/model/ArticleScraper/ArticleScraper.hpp` (C++ port of `Readability.js`,
adapted for lexbor).

Copyright (c) 2010 Arc90 Inc
Copyright (c) 2010-2026 Mozilla and Contributors

Licensed under the Apache License, Version 2.0. A copy of the license is
included in [LICENSES/Apache-2.0.txt](LICENSES/Apache-2.0.txt).

NOTICE:

```text
Readability
Copyright (c) 2010 Arc90 Inc
Copyright (c) 2010-2026 Mozilla and Contributors
```

Changes from the original: ported from JavaScript to C++20, replaced the DOM
implementation with lexbor, and adapted the output to hn-reader's
`ContentBlock` model.
