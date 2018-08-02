package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"os"
	"strconv"
)

func main() {
	var buf bytes.Buffer
	var content_length, err = strconv.ParseInt(os.Getenv("CONTENT_LENGTH"), 10, 64)
	var r = bufio.NewReader(os.Stdin)
	var count int64 = 0
	if err == nil {
		for {
			c, err := r.ReadByte()
			if err == io.EOF ||
				count == content_length {
				break
			}
			buf.WriteByte(c)
			count++
		}
	}

	fmt.Printf("Content-type: text/html\r\n")
	fmt.Printf("\r\n")
	fmt.Printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n")
	fmt.Printf("<html>\n")
	fmt.Printf("<head>\n")
	//fmt.Printf("<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n")
	fmt.Printf("<META http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n")
	fmt.Printf("<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n")
	fmt.Printf("<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n")
	fmt.Printf("</head>\n")

	fmt.Printf("<body>\n")

	fmt.Printf("Moai CGI Post test(golang).\n")
	fmt.Printf("<form action=\"post_go.go\" method=\"POST\">\n")
	fmt.Printf("<b>E-mail</b><input type=text name=email size=\"28\"><br>\n")
	fmt.Printf("<b>コメント</b><textarea name=com cols=\"48\" rows=\"4\"></textarea><br>\n")
	fmt.Printf("<input type=submit value=\"返信する\">\n")
	fmt.Printf("</form>\n")

	fmt.Printf("content_length=[%d]\n<br>", content_length)
	fmt.Printf("Original Body=[%s]\n", buf.String())
	fmt.Printf("</body></html>\n")
}
