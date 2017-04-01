<?xml version="1.0" encoding="iso-8859-1"?><xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <html>
      <body>
        <xsl:for-each select="//ipc_session">
          <h3><xsl:if test="@name"><xsl:value-of select="@name"/>&#160;</xsl:if><xsl:if test="@type"><xsl:value-of select="@type"/>&#160;</xsl:if>session #<xsl:value-of select="position()"/>:</h3>
          <xsl:for-each select="ipc_event">
            <xsl:sort select="timestamp"/>
            <xsl:sort select="serial_num"/>
            <xsl:sort select="direction" order="descending"/>
            <xsl:variable name="year" select="substring-before(timestamp, '-')"/>
            <xsl:variable name="month" select="substring-before(substring(timestamp, string-length($year) + 2), '-')"/>
            <xsl:variable name="day" select="substring-before(substring(timestamp, string-length($month) + string-length($year) + 3), 'T')"/>
            <xsl:variable name="time" select="substring-after(timestamp, 'T')"/>
            <xsl:value-of select="$month"/>/<xsl:value-of select="$day"/>/<xsl:value-of select="$year"/>&#160;<xsl:value-of select="$time"/>
            <xsl:choose>
              <xsl:when test="type = 'message'">
                &#160;- IPC Message:&#160;
              </xsl:when>
              <xsl:otherwise>
                &#160;- IPC Event [<xsl:value-of select="type"/>
                <xsl:if test="event_desc">:&quot;<xsl:value-of select="event_desc"/>&quot;</xsl:if>]:&#160;
              </xsl:otherwise>
            </xsl:choose>
            (<b><xsl:value-of select="direction"/></b><xsl:if test="receive_type">:<xsl:value-of select="receive_type"/></xsl:if>)
          
            <table border="0" cellspacing="0">
              <xsl:if test="serial_num">
                <tr>
                  <td>&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;</td>
                  <td>Message Num:&#160;</td>
                  <td><xsl:value-of select="serial_num"/></td>
                </tr>
              </xsl:if>
              <tr>
                <td>&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;</td>
                <td>Transport:&#160;</td>
                <td><xsl:value-of select="transport"/></td>
              </tr>
              <tr>
                <td>        </td>
                <td>Proc Name (ID):&#160;</td>
                <td><xsl:value-of select="proc_name"/> (<xsl:value-of select="proc_id"/>)</td>
              </tr>
              <xsl:if test="msg_id != 0">
                <tr>
                  <td>        </td>
                  <td>ID:&#160;</td>
                  <td><xsl:value-of select="msg_id"/><xsl:if test="msg_name"> (<xsl:value-of select="msg_name"/>)</xsl:if></td>
                </tr>
              </xsl:if>
              <xsl:if test="from">
                <tr>
                  <td>        </td>
                  <td>From:&#160;</td>
                  <td><xsl:value-of select="from"/></td>
                </tr>
              </xsl:if>
              <xsl:if test="to">
                <tr>
                  <td>        </td>
                  <td>To:&#160;</td>
                  <td><xsl:value-of select="to"/></td>
                </tr>
              </xsl:if>
              <xsl:if test="job_name">
                <tr>
                  <td>        </td>
                  <td>Job Name:&#160;</td>
                  <td><xsl:value-of select="job_name"/></td>
                </tr>
              </xsl:if>
              <xsl:if test="project_name">
                <tr>
                  <td>        </td>
                  <td>Project Name:&#160;</td>
                  <td><xsl:value-of select="project_name"/></td>
                </tr>
              </xsl:if>
              <xsl:if test="message_unique_name">
                <tr>
                  <td>        </td>
                  <td>Unique Message Name:&#160;</td>
                  <td><xsl:value-of select="message_unique_name"/></td>
                </tr>
              </xsl:if>
              <xsl:if test="data">
                <tr>
                  <td>        </td>
                  <td><b>Message Data:&#160;</b></td>
                </tr>
                <xsl:for-each select="data/item">
                  <tr>
                    <td>        </td>
                    <td><xsl:value-of select="name"/>:</td>
                    <td><xsl:copy-of select="value"/></td>
                  </tr>
                </xsl:for-each>
              </xsl:if>
            </table><br/>
          </xsl:for-each>
        </xsl:for-each>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
