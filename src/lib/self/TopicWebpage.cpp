// last modified 

#include "TopicWebpage.h"
#include <iostream>
#include <sstream>
#include "../helper/Misc.h"

#include <unistd.h>

using namespace std;


TopicWebpage::TopicWebpage ( const string& url,
                             ParsePicturesUrls parsePicturesUrls,
                             ParseSeedUrl parseSeedUrl,
                             const string& proxy_addr,
                             const string& src_charset,
                             const string& dest_charset )
    : Webpage(url, "", proxy_addr)
{
    if (!isLoaded()) {
        return;
    }

    // charset convert
    if (!src_charset.empty() && !dest_charset.empty()) {
        convertCharset(src_charset, dest_charset);
    }

    // parse the URLs of av pictures
    if (!parsePicturesUrls(getTxt(), pictures_urls_list_)) {
        //cerr << "WARNING! parsePicturesUrls() failure from " << url << endl;
        ;
    }

    // parse the URLs of seed
    if (!parseSeedUrl(getTxt(), seed_url_)) {
        //cerr << "WARNING! parseSeedUrl() failure from " << url << endl;
        ;
    }
}

TopicWebpage::~TopicWebpage ()
{
    ;
}

const vector<string>&
TopicWebpage::getPicturesUrlsList (void) const
{
    return(pictures_urls_list_);
}

const string&
TopicWebpage::getSeedUrl (void) const
{
    return(seed_url_);
}

// the name rule of pictures: topictitle0.jpg, topictitle1.jpg, topictitle[x].jpg
bool
TopicWebpage::downloadAllPictures ( const string& path,
                                    const string& base_name,
                                    unsigned timeout_download_pic,
                                    vector<string>& fail_download_pics_urls_list,
                                    unsigned pictures_max_num )
{
    fail_download_pics_urls_list.clear();

    for ( unsigned i = 0, sucess_cnt = 0;
          i < pictures_urls_list_.size() && sucess_cnt < pictures_max_num;
          ++i ) {
        const string& picture_url = pictures_urls_list_[i];
        
        // make picture filename
        const string tmp = getRemoteFiletype(picture_url);
        static const string keyword("image/");
        string postfix_name;
        const auto pos = tmp.find(keyword);
        if (string::npos != pos) {
            postfix_name = tmp.substr(pos + keyword.size());
        }
        
        // download pic
        const string pic_filename = path + "/" + base_name + "-" + convNumToStr(i) + "." + postfix_name;
        if (downloadFile(picture_url, pic_filename, "", timeout_download_pic)) {
            ++sucess_cnt;
            continue;
        }
        
        //cerr << "WARNING! CANNOT download " << pictures_urls_list_[i] << endl;
        fail_download_pics_urls_list.push_back(pictures_urls_list_[i]);
        remove(pic_filename.c_str());
    }


    return(fail_download_pics_urls_list.empty());
}
