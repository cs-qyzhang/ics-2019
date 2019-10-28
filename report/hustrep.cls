%
% hustrep.cls
% 华中科技大学报告模板
% 
% Created in 2018/9/22 by 张丘洋
%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{hustrep}[2019/10/28 v0.5 HUST Report Class]
\LoadClass[a4paper,openright]{book}

% 宏包声明
\RequirePackage[UTF8,zihao=-4,fontset=none]{ctex}
\RequirePackage{CJKfntef}%汉字居中对齐环境：CJKfilltwosides;汉字下划线：CJKunderline
\RequirePackage{makecell,array,multirow}
\RequirePackage{titlesec,graphicx,subcaption,geometry,fancyhdr,xcolor,listings}%
\RequirePackage{enumitem}%
\RequirePackage{tikz}
\RequirePackage{hyperref}
\RequirePackage{booktabs}
\RequirePackage[format=hang,skip=5pt]{caption}
\RequirePackage{amsthm,amssymb,amsmath,amsfonts,dsfont}
\RequirePackage{thmtools}
\RequirePackage{tcolorbox}
\RequirePackage[newfloat]{minted}
\RequirePackage{newfloat}

% hyperref 配置
\hypersetup{hidelinks}

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
  \pagenumbering{arabic}
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
  \pagenumbering{arabic}
}

% 页边距设置
\geometry{left=3cm,right=2cm,bottom=3cm,top=3cm}
\raggedbottom

% ctex 配置
\ctexset{%
  punct   = quanjiao,%
  space   = auto,%
  today   = small%
}

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
\setmainfont{Nimbus Roman}

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
  {\dabiaosong\fontsize{30}{36} \makebox[0.65\textwidth][s]{课程实验报告} \par}
  \vfill
  {\LARGE\xiaobiaosong 课程名称：\fixedunderline{9cm}{\makebox[7cm][s]{\@title}}\par}
  \vfill
  {\large\xiaobiaosong 姓　　名：\fixedunderline{7cm}{\makebox[5em][s]{\@author}}\par\vspace{2ex}}
  {\large\xiaobiaosong 学　　号：\fixedunderline{7cm}{\hust@studentid}}\par\vspace{2ex}
  {\large\xiaobiaosong 院　　系：\fixedunderline{7cm}{\hust@college}}\par\vspace{2ex}
  {\large\xiaobiaosong 专业班级：\fixedunderline{7cm}{\hust@class}}\par\vspace{2ex}
  {\large\xiaobiaosong 指导教师：\fixedunderline{7cm}{\makebox[5em][s]{\hust@teacher}}}\par\vspace{2ex}
  \vfill
  \hspace*{18em}
  \renewcommand\arraystretch{2}
  \begin{tabular}{|c|p{8em}|}
    \hline
    分数     & \\
    \hline
    教师签名 & \\
    \hline
  \end{tabular}
  \vfill
  {\large\bfseries\today}
  \renewcommand\arraystretch{1.3}
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

%tikz样式定义
\usetikzlibrary{calc,decorations.pathmorphing,positioning,backgrounds,fit,arrows,shapes.geometric,graphs,intersections}
\tikzstyle{mealystate}=[fill=blue!20,font=\ttfamily\small,draw=blue!50,minimum size=3em,inner sep=3pt,circle]
\tikzstyle{moorestate}=[fill=blue!20,font=\ttfamily\small,draw=blue!50,minimum size=1em,inner sep=5pt,circle split]
\tikzstyle{trans}=[->]
\tikzstyle{startstop}=[rectangle,rounded corners=5pt,minimum width=4em,inner sep=0.5em,text centered,align=center,draw=black,fill=blue!20]
\tikzstyle{io}=[trapezium,trapezium left angle=70,trapezium right angle=110,minimum width=4em,inner sep=0.5em,text centered,draw=black,fill=blue!20]
\tikzstyle{process}=[rectangle,minimum width=4em,inner sep=0.5em,text centered,draw=black,fill=blue!20]
\tikzstyle{decision}=[diamond,shape aspect=2,minimum width=6em,inner sep=3pt,text centered,draw=black,fill=blue!20]
\tikzstyle{yesno}=[auto,near start]
\tikzstyle{middle}=[circle,minimum size=1em,inner sep=0.5em,draw=black,fill=blue!20]
\tikzset{%
  line width=1pt,%
  >=stealth%
}

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
