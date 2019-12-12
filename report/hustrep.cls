%
% hustrep.cls
% 华中科技大学报告模板
% 
% Created in 2018/9/22 by 张丘洋
%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{hustrep}[2019/10/28 v0.7 HUST Report Class]
\LoadClass[a4paper,openright]{book}

% 宏包声明
\RequirePackage[UTF8,zihao=-4]{ctex}% 加上fontset=none时使用lualatex正文字体会出错
\RequirePackage{CJKfntef}%汉字居中对齐环境：CJKfilltwosides;汉字下划线：CJKunderline
\RequirePackage{makecell,array,multirow}
\RequirePackage{titlesec,graphicx,subcaption,geometry,fancyhdr,xcolor,listings}%
\RequirePackage{enumitem}%
\RequirePackage{tikz}
\RequirePackage[hidelinks]{hyperref}
\RequirePackage{booktabs}
\RequirePackage[format=hang,skip=5pt]{caption}
\RequirePackage{amsthm,amssymb,amsmath,amsfonts}
\RequirePackage{thmtools}
\RequirePackage{tcolorbox}
\RequirePackage[newfloat]{minted}
\RequirePackage{newfloat}
\RequirePackage{fontawesome}

% 字体配置
\setCJKmainfont[%
  BoldFont=Source Han Serif CN Bold,%
  ItalicFont=FZKai-Z03S,%
  BoldItalicFont=FZCuKaiS-R-GB]{Source Han Serif CN}
\setCJKsansfont[%
  BoldFont=Source Han Sans CN Bold,%
  ItalicFont=FZKai-Z03S,%
  BoldItalicFont=FZCuKaiS-R-GB]{Source Han Sans CN}
\setCJKmonofont[%
  BoldFont=Source Han Sans CN Bold,%
  ItalicFont=FZKai-Z03S,%
  BoldItalicFont=FZCuKaiS-R-GB]{Source Han Sans CN}
\newCJKfontfamily\kai[BoldFont=FZCuKaiS-R-GB]{FZKai-Z03S}
\newCJKfontfamily\xiaobiaosong[AutoFakeBold]{FZXiaoBiaoSong-B05S}
\newCJKfontfamily\dabiaosong[AutoFakeBold]{FZDaBiaoSong-B06S}
\setmainfont{TeX Gyre Termes}

%字段定义
\def\hust@class{计算机科学与技术1606班}%
\def\hust@studentid{U201614667}%
\def\hust@teacher{}%
\def\hust@college{计算机科学与技术学院}%
\def\hust@yemei{\@title 实验报告}%
\newcommand\class[1]{\def \hust@class {#1}}%
\newcommand\studentid[1]{\def \hust@studentid {#1}}%
\newcommand\teacher[1]{\def \hust@teacher {#1}}
\newcommand\yemei[1]{\def \hust@yemei {#1}}
\newcommand\college[1]{\def \hust@college {#1}}
\let\oldtableofcontents\tableofcontents
\renewcommand{\tableofcontents}{%
  \titleformat{\chapter}[display]
              {\Huge\dabiaosong}
              {\filleft\dabiaosong\chaptertitlename \zihao{0}\thechapter}
              {4ex}{\titlerule\vspace{2ex}\filright}
              [\vspace{2ex}\titlerule]
  \pagenumbering{Roman}%
  \oldtableofcontents%
  \titleformat{\chapter}[display]
              {\Huge\dabiaosong}
              {\filleft\dabiaosong\chaptertitlename \zihao{0}\thechapter\thispagestyle{empty}}
              {4ex}{\titlerule\vspace{2ex}\filright}
              [\vspace{2ex}\titlerule\clearpage]
  \chapterend%
  \pagenumbering{arabic}%
  \pagestyle{main}%
}
\let\oldlstlistoflistings\lstlistoflistings
\renewcommand{\lstlistoflistings}{%
  \titleformat{\chapter}[display]
              {\Huge\dabiaosong}
              {\filleft\dabiaosong\chaptertitlename \zihao{0}\thechapter}
              {4ex}{\titlerule\vspace{2ex}\filright}
              [\vspace{2ex}\titlerule]
  \pagenumbering{Roman}%
  \oldlstlistoflistings%
  \titleformat{\chapter}[display]
              {\Huge\dabiaosong}
              {\filleft\dabiaosong\chaptertitlename \zihao{0}\thechapter\thispagestyle{empty}}
              {4ex}{\titlerule\vspace{2ex}\filright}
              [\vspace{2ex}\titlerule\clearpage]
  \chapterend%
  \pagenumbering{arabic}%
  \pagestyle{main}%
}

% 页边距设置
\geometry{left=3cm,right=2cm,bottom=3cm,top=3cm}
\raggedbottom

% ctex 配置
\ctexset{%
  punct      = quanjiao,%
  space      = auto,%
  today      = small,%
  autoindent = true%
}

%取消列表项的间距
\setlist{nosep,labelindent=\parindent,leftmargin=0pt,itemindent=*,listparindent=\parindent}
\setlist[enumerate,1]{label=(\arabic*).,font=\textup}
\setlist[enumerate,2]{label=\alph*.,font=\textup,labelindent=2\parindent,leftmargin=\parindent,itemindent=*}

% 设置目录及标题
\setcounter{tocdepth}{1}%设置目录显示深度
\renewcommand{\chaptername}{}
\renewcommand{\partname}{}
\titleformat{\chapter}[display]
            {\Huge\dabiaosong}
            {\filleft\dabiaosong\chaptertitlename \zihao{0}\thechapter\thispagestyle{empty}}
            {4ex}{\titlerule\vspace{2ex}\filright}
            [\vspace{2ex}\titlerule\clearpage]
%\titleformat{\chapter}{\raggedright\Huge\dabiaosong}{\thechapter}{1ex}{}%设置Chapter格式
\titleformat{\section}{\raggedright\Large\xiaobiaosong}{\thesection}{1ex}{}
\titleformat{\subsection}{\raggedright\large\xiaobiaosong}{\thesubsection}{1ex}{}
\newcommand{\fixedunderline}[2]{\underline{\makebox[#1]{#2}}}%固定长度下划线

\newcommand{\code}[1]{{\texttt{#1}}}
\newcommand{\ccode}[1]{\mintinline{C}{#1}}
\newcommand{\file}[1]{{\texttt{#1}}}

% 设置正文页眉页脚
\setlength{\headheight}{15pt}
\renewcommand{\headrulewidth}{0pt}
\fancypagestyle{main}{%
  \fancyhf{}
  \lhead[\kai \hust@yemei]{\kai\rightmark}
  \rhead[\kai\rightmark]{\kai \hust@yemei}
  \lfoot[\thepage]{}
  \rfoot[]{\thepage}
  \cfoot[]{}
}

% 设置空白页面页眉页脚格式
\fancypagestyle{plain}{%
  \fancyhf{}
  \lfoot[\thepage]{}
  \rfoot[]{\thepage}
}

\fancypagestyle{empty}{%
  \fancyhf{}%
}

% 章节末尾清空页面命令定义
\newcommand{\chapterend}{%
  \clearpage{\thispagestyle{empty}\cleardoublepage}%
}%
\newcommand{\Chapter}[1]{%
  \chapter{#1}{\thispagestyle{empty}\clearpage}%
}%
% 自动在章节尾添加\chapterend命令
\let\stdchapter\chapter%
\renewcommand\chapter{\ifnum\c@chapter>0{\chapterend}\else\fi\stdchapter}%

% maketitle命令绘制首页
\renewcommand{\maketitle}{%
  \begin{titlepage}
  \centering
  \begin{figure}[h]
    \centering
    \begin{subfigure}[c]{0.4\textwidth}
      \raggedleft%
      \includegraphics[width=0.6\textwidth]{figure/xiaohui.png}
    \end{subfigure}%
    \begin{subfigure}[c]{0.6\textwidth}
      \raggedright%
      \includegraphics[width=0.7\textwidth]{figure/xiaozi.png}
    \end{subfigure}%
  \end{figure}%
  \vspace{1cm}
  {\dabiaosong\fontsize{30}{36}\selectfont \makebox[0.65\textwidth][s]{课程实验报告} \par}
  \vfill
  {\LARGE\xiaobiaosong 课程名称：\fixedunderline{9cm}{\makebox[7cm][s]{\@title}}\par}
  \vfill
  {\large\xiaobiaosong 姓　　名：\fixedunderline{7cm}{\makebox[5em][s]{\@author}}\par\vspace{2ex}}
  {\large\xiaobiaosong 学　　号：\fixedunderline{7cm}{\hust@studentid}}\par\vspace{2ex}
  {\large\xiaobiaosong 院　　系：\fixedunderline{7cm}{\hust@college}}\par\vspace{2ex}
  {\large\xiaobiaosong 专业班级：\fixedunderline{7cm}{\hust@class}}\par\vspace{2ex}
  {\large\xiaobiaosong 指导教师：\fixedunderline{7cm}{\makebox[5em][s]{\hust@teacher}}}\par\vspace{2ex}
  \vfill
  \hspace*{15em}
  \begin{tikzpicture}[every node/.style={minimum height=3em,line width=.8pt}]
    \node[minimum width=6em] at (0,0)             (1) {分数};
    \node[minimum width=6em,below=0pt of 1]       (2) {教师签名};
    \node[minimum width=8em,right=0pt of 1]       (3) {};
    \node[minimum width=8em,right=0pt of 2]       (4) {};
    \draw (1.north west) rectangle (4.south east);
    \draw (1.south west) -- (4.north east);
    \draw (1.north east) -- (4.south west);
  \end{tikzpicture}
  \vfill
  {\large\bfseries\today}
  \end{titlepage}%
  \chapterend%
  \pagestyle{plain}%
}

% listing 环境配置
\DeclareCaptionFormat{listing}{\rule{\dimexpr\textwidth\relax}{1.2pt}\par\vskip1pt#1#2#3}
\captionsetup[lstlisting]{format=listing,justification=centering,singlelinecheck=false,margin=0pt,textfont={it},labelsep=space}
\renewcommand{\lstlistlistingname}{程序清单列表}
\renewcommand{\lstlistingname}{\kai 程序清单}
\lstset{%
  language=SQL,%
  basicstyle=\ttfamily\small,%
  numbers=left,%
  numberstyle=\tiny,%
  breaklines=true,%
  belowcaptionskip=1.5ex,%
  keywordstyle=\color{blue!70},%
  commentstyle=\ttfamily\color{red!50!green!50!blue!50},%
  identifierstyle=\color{purple},%
  stringstyle=\color{orange},%
  backgroundcolor=\color{gray!10},%
  framerule=1.2pt,%
  frame=lines%
  %rulesepcolor=\color{red!20!green!20!blue!20},
}

% 带圈数字定义
\newcommand\circled[1]{{\large \textcircled{\small #1}}}%

% 设置表格间距
\renewcommand\arraystretch{1.3}

% 图片环境
\newcommand\hustfigure[4][0.7\textwidth]{%
  \begin{figure}[!htbp]%
    \centering%
    \includegraphics[width=#1]{#2}%
    \caption{#3}%
    \label{#4}%
  \end{figure}%
}%
\newcommand\hustfiguretwo[6]{%
  \begin{figure}[!htbp]%
    \parbox[b]{0.5\textwidth}{%
      \centering%
      \includegraphics[width=0.4\textwidth]{#1}%
      \caption{#2}%
      \label{#3}%
    }%
    \parbox[b]{0.5\textwidth}{%
      \centering%
      \includegraphics[width=0.4\textwidth]{#4}%
      \caption{#5}%
      \label{#6}%
    }%
  \end{figure}%
}
\setlength{\intextsep}{0.5em plus 2pt minus 2pt}
\setlength{\textfloatsep}{0.5em plus 2pt minus 2pt}
\setlength{\floatsep}{0.5em plus 2pt minus 2pt}

\renewcommand\emph[1]{{\bf\tt #1}}

% 设置定义环境
\RequirePackage{amsthm}
\RequirePackage{thmtools}
\declaretheorem[shaded={rulecolor=black,rulewidth=2pt,bgcolor={rgb}{1,1,1}},title=定义]{Definition}
\renewcommand\listtheoremname{定义列表}

\tcbset{%
  before skip=10pt,
  after skip=10pt
}
% 用于显示类成员和方法的盒子
\tcbuselibrary{minted,skins,breakable}
\newtcolorbox{classbox}[1]{%
  colback=gray!5,%
  colframe=gray!50!black,%
  subtitle style={boxrule=0.4pt,colback=gray!85!black},%
  breakable,%
  title=\large#1%
}%

% 代码
\newtcblisting[auto counter,number within=chapter]{codes}[4][]{
  listing engine=minted,
  list entry={\protect\numberline{\thetcbcounter}#3},
  breakable,
  title=\kai 程序清单\thetcbcounter: #3,
  label=#4,
  after title={\hfill\colorbox{gray!75}{#2}},
  colback=gray!5,%
  colframe=gray!50!black,%
  listing only,
  minted style=friendly,
  minted language=#2,
  minted options={%
    linenos=true,
    numbersep=2.5mm,
    breaklines=true,
    fontsize=\small,
    escapeinside=\@\@,
    mathescape=true,
    %baselinestretch=1.1,
  },
  left=5mm,
  top=0mm,
  bottom=0mm,
  enhanced,
  overlay={\begin{tcbclipinterior}\fill[gray!25] (frame.south west)
            rectangle ([xshift=5mm]frame.north west);\end{tcbclipinterior}},
  #1
}

% 无标题代码
\newtcblisting{codes*}[2][]{
  listing engine=minted,
  breakable,
  colback=gray!5,%
  colframe=gray!50!black,%
  listing only,
  minted style=friendly,
  minted language=#2,
  minted options={%
    linenos=true,
    numbersep=2.5mm,
    breaklines=true,
    fontsize=\small,
    escapeinside=\@\@,
    mathescape=true,
  },
  left=5mm,
  top=0mm,
  bottom=0mm,
  enhanced,
  overlay={\begin{tcbclipinterior}\fill[gray!25] (frame.south west)
            rectangle ([xshift=5mm]frame.north west);\end{tcbclipinterior}},
  #1
}

\newcommand*\keystroke[1]{%
  \tikz[baseline=(key.base)]
    \node[%
      draw=white!20!gray!10,
      fill=lightgray!80,
      drop shadow={shadow xshift=0.25ex,shadow yshift=-0.25ex,fill=lightgray,opacity=0.4},
      rectangle,
      rounded corners=2pt,
      inner sep=1pt,
      line width=0.5pt,
      font=\scriptsize\sffamily
    ](key) {#1\strut}
  ;
}

\definecolor{crimson}{rgb}{0.86, 0.08, 0.24}
% https://tex.stackexchange.com/a/340935
\newtcolorbox{mybox}[4][]{%
  enhanced,
  breakable,
  colback=#2!5,
  colframe=#2,
  attach boxed title to top left={yshift=-2pt}, title={\hspace*{.5mm} #3},
  boxrule=0pt,
  size=title,
  top=1.5mm,
  bottom=1.5mm,
  boxed title style={%
    sharp corners,
    rounded corners=northwest,
    colback=tcbcolframe,
    top=3pt,
    bottom=3pt,
    boxrule=0pt
  },
  sharp corners=north,
  overlay unbroken={%
    \path[fill=tcbcolframe]
      (title.south east)
      to[out=0, in=180] ($(title.east)+(1cm,0.25cm)$)--
      ($(title.east-|frame.north east)+(0,0.25cm)$) [rounded corners=\kvtcb@arc] |- ([xshift=-1pt]title.north east);
    \path[fill=tcbcolback]
      (title.south east)
      to[out=0, in=180] ($(title.east)+(1cm,0.25cm)$)--
      ($(title.east-|frame.north east)+(0,0.25cm)$) |- (title.south east);
    \path[draw=tcbcolframe,line width=0.4mm]
      ([yshift=-3pt,xshift=-0.2mm]frame.north east) --
      ($(title.east-|frame.north east)+(-0.2mm,0.26cm)$);
    \foreach \x in {10,20,...,100}
      \path[opacity=\x*0.01,line width=1.1pt,draw=gray!\x]
        ($([xshift=.4mm]title.south west)+(0,\x/100pt-1.1pt)$) --
        ($(title.south east)+(0,\x/100pt-1.1pt)$) to[out=0, in=180]
        ($(title.east)+(1cm,0.25cm+\x/100pt-1.1pt)$) --
        ($(title.east-|frame.north east)+(-0.4mm,0.25cm+\x/100pt-1.1pt)$);
    \path[draw=white,line width=1.1pt]
      ([xshift=.4mm]title.south west) -- (title.south east) to[out=0, in=180] ($(title.east)+(1cm,0.25cm)$)--
      ($(title.east-|frame.north east)+(-0.4mm,0.25cm)$);
    \node[anchor=south east,minimum height=20pt,inner xsep=5pt] at ([yshift=-5pt]frame.north east)
      {\color{tcbcolframe} #4 \hspace*{1.5mm}};
    \draw[line width=.4mm, rounded corners=\kvtcb@arc, 
      tcbcolframe] 
      (title.north west) rectangle 
      (frame.south east);
  },
  overlay first={%
    \path[fill=tcbcolframe]
      (title.south east)
      to[out=0, in=180] ($(title.east)+(1cm,0.25cm)$)--
      ($(title.east-|frame.north east)+(0,0.25cm)$) [rounded corners=\kvtcb@arc] |- ([xshift=-1pt]title.north east);
    \path[fill=tcbcolback]
      (title.south east)
      to[out=0, in=180] ($(title.east)+(1cm,0.25cm)$)--
      ($(title.east-|frame.north east)+(0,0.25cm)$) |- (title.south east);
    \path[draw=tcbcolframe,line width=0.4mm]
      ([yshift=-3pt,xshift=-0.2mm]frame.north east) --
      ($(title.east-|frame.north east)+(-0.2mm,0.26cm)$);
    \foreach \x in {10,20,...,100}
      \path[opacity=\x*0.01,line width=1.1pt,draw=gray!\x]
        ($([xshift=.4mm]title.south west)+(0,\x/100pt-1.1pt)$) --
        ($(title.south east)+(0,\x/100pt-1.1pt)$) to[out=0, in=180]
        ($(title.east)+(1cm,0.25cm+\x/100pt-1.1pt)$) --
        ($(title.east-|frame.north east)+(-0.4mm,0.25cm+\x/100pt-1.1pt)$);
    \path[draw=white,line width=1.1pt]
      ([xshift=.4mm]title.south west) -- (title.south east) to[out=0, in=180] ($(title.east)+(1cm,0.25cm)$)--
      ($(title.east-|frame.north east)+(-0.4mm,0.25cm)$);
    \node[anchor=south east,minimum height=20pt,inner xsep=5pt] at ([yshift=-5pt]frame.north east)
      {\color{tcbcolframe} #4 \hspace*{1.5mm}};
    \draw[line width=.4mm, rounded corners=\kvtcb@arc, 
      tcbcolframe] 
      (frame.south west) |- (title.north) -| 
      (frame.south east);
  },
  overlay middle={%
    \draw[line width=.4mm, tcbcolframe] 
    (frame.north west)--(frame.south west) 
    (frame.north east)--(frame.south east);
  },
  overlay last={%
    \draw[line width=.4mm, rounded corners=\kvtcb@arc, 
      tcbcolframe] 
      (frame.north west) |- (frame.south) -|
      (frame.north east);
  },
  #1
}

\newcommand{\bugbox}[2]{%
  \begin{mybox}{crimson}{\faBug\ \faBug\ \faBug}{\file{#1}}%
    #2%
  \end{mybox}%
}
