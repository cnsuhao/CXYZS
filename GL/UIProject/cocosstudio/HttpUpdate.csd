<GameProjectFile>
  <PropertyGroup Type="Scene" Name="HttpUpdate" ID="12c686d6-3fe8-48b3-869e-53269819922b" Version="2.0.2.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="Scene" CanEdit="False" FrameEvent="" ctype="SingleNodeObjectData">
        <Position X="0.0000" Y="0.0000" />
        <Scale ScaleX="1.0000" ScaleY="1.0000" />
        <AnchorPoint />
        <CColor A="255" R="255" G="255" B="255" />
        <Size X="1024.0000" Y="615.0000" />
        <PrePosition X="0.0000" Y="0.0000" />
        <PreSize X="0.0000" Y="0.0000" />
        <Children>
          <NodeObjectData Name="Sprite_2" ActionTag="1775262949" FrameEvent="" Tag="17" ObjectIndex="2" ctype="SpriteObjectData">
            <Position X="0.0000" Y="0.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="1024.0000" Y="615.0000" />
            <PrePosition X="0.0000" Y="0.0000" />
            <PreSize X="0.0000" Y="0.0000" />
            <FileData Type="Normal" Path="Login/LoginCanvas.jpg" />
          </NodeObjectData>
          <NodeObjectData Name="Sprite_1" ActionTag="1399935315" FrameEvent="" Tag="488" ObjectIndex="1" ctype="SpriteObjectData">
            <Position X="512.0000" Y="100.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="640.0000" Y="22.0000" />
            <PrePosition X="0.5000" Y="0.1626" />
            <PreSize X="0.0000" Y="0.0000" />
            <FileData Type="Normal" Path="HttpUpdate/HttpUpdate_A.png" />
          </NodeObjectData>
          <NodeObjectData Name="LoadingBar" ActionTag="-540855209" FrameEvent="" Tag="487" ObjectIndex="1" ProgressInfo="60" ctype="LoadingBarObjectData">
            <Position X="512.0000" Y="100.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="582.0000" Y="14.0000" />
            <PrePosition X="0.5000" Y="0.1626" />
            <PreSize X="0.0000" Y="0.0000" />
            <ImageFileData Type="Normal" Path="HttpUpdate/HttpUpdate_B.png" />
          </NodeObjectData>
          <NodeObjectData Name="Text_1" ActionTag="-2119007743" FrameEvent="" Tag="489" ObjectIndex="1" FontSize="18" LabelText="下载进度:" ctype="TextObjectData">
            <Position X="819.4700" Y="129.2700" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="81.0000" Y="18.0000" />
            <PrePosition X="0.8003" Y="0.2102" />
            <PreSize X="0.0000" Y="0.0000" />
          </NodeObjectData>
          <NodeObjectData Name="Text_3" ActionTag="-1260354837" FrameEvent="" Tag="491" ObjectIndex="3" FontSize="18" LabelText="当前版本:" ctype="TextObjectData">
            <Position X="121.6542" Y="149.7088" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="81.0000" Y="18.0000" />
            <PrePosition X="0.1188" Y="0.2434" />
            <PreSize X="0.0000" Y="0.0000" />
          </NodeObjectData>
          <NodeObjectData Name="Text_4" ActionTag="-1558079408" FrameEvent="" Tag="493" ObjectIndex="5" FontSize="18" LabelText="最新版本:" ctype="TextObjectData">
            <Position X="121.6543" Y="123.7093" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="81.0000" Y="18.0000" />
            <PrePosition X="0.1188" Y="0.2012" />
            <PreSize X="0.0000" Y="0.0000" />
          </NodeObjectData>
          <NodeObjectData Name="currentVersionText" ActionTag="-1586710993" FrameEvent="" Tag="492" ObjectIndex="4" FontSize="18" LabelText="1.00" IsCustomSize="True" ctype="TextObjectData">
            <Position X="195.0000" Y="149.7097" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="50.0000" Y="18.0000" />
            <PrePosition X="0.1904" Y="0.2434" />
            <PreSize X="0.0488" Y="0.0293" />
          </NodeObjectData>
          <NodeObjectData Name="lastVersionText" ActionTag="-1257009960" FrameEvent="" Tag="494" ObjectIndex="6" FontSize="18" LabelText="1.00" IsCustomSize="True" ctype="TextObjectData">
            <Position X="195.0000" Y="123.7099" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="50.0000" Y="18.0000" />
            <PrePosition X="0.1904" Y="0.2012" />
            <PreSize X="0.0488" Y="0.0293" />
          </NodeObjectData>
          <NodeObjectData Name="downloadProgressBarText" ActionTag="256497619" FrameEvent="" Tag="490" ObjectIndex="2" FontSize="16" LabelText="32750/55526" ctype="TextObjectData">
            <Position X="512.0000" Y="119.2523" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="0" />
            <Size X="88.0000" Y="16.0000" />
            <PrePosition X="0.5000" Y="0.1939" />
            <PreSize X="0.0000" Y="0.0000" />
          </NodeObjectData>
          <NodeObjectData Name="downloadRateText" ActionTag="-1927584285" FrameEvent="" Tag="495" ObjectIndex="7" FontSize="18" LabelText="80" ctype="TextObjectData">
            <Position X="873.8611" Y="129.2700" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <CColor A="255" R="255" G="255" B="255" />
            <Size X="18.0000" Y="18.0000" />
            <PrePosition X="0.8534" Y="0.2102" />
            <PreSize X="0.0000" Y="0.0000" />
          </NodeObjectData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameProjectFile>