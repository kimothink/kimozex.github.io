<div id="kboard-customer-editor">
 <form class="kboard-form" method="post" action="<?php echo $url->getContentEditorExecute()?>" enctype="multipart/form-data" onsubmit="return kboard_editor_execute(this);">
 
   <?php wp_nonce_field('kboard-editor-execute', 'kboard-editor-execute-nonce')?>
  <input type="hidden" name="action" value="kboard_editor_execute">
  <input type="hidden" name="mod" value="editor">
  <input type="hidden" name="uid" value="<?php echo $content->uid?>">
  <input type="hidden" name="board_id" value="<?php echo $content->board_id?>">
  <input type="hidden" name="parent_uid" value="<?php echo $content->parent_uid?>">
  <input type="hidden" name="member_uid" value="<?php echo $content->member_uid?>">
  <input type="hidden" name="member_display" value="<?php echo $content->member_display?>">
  <input type="hidden" name="date" value="<?php echo $content->date?>">
  <input type="hidden" name="user_id" value="<?php echo get_current_user_id()?>">
  
  <div class="kboard-attr-row kboard-attr-title">
   <label class="attr-name" for="kboard-input-title"><?php echo __('Title', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-title" name="title" value="<?php echo $content->title?>" placeholder="<?php echo __('Title', 'kboard')?>..."></div>
  </div>

<div class="kboard-attr-row">
   <div class="attr-name"><?php echo __('Options', 'kboard')?></div>
   <div class="attr-value">
    <label class="attr-value-option"><input type="checkbox" name="secret" value="true" onchange="kboard_toggle_password_field(this)"<?php if($content->secret):?> checked<?php endif?>> <?php echo __('Secret', 'kboard')?></label>
    <?php if($board->isAdmin()):?>
    <label class="attr-value-option"><input type="checkbox" name="notice" value="true"<?php if($content->notice):?> checked<?php endif?>> <?php echo __('Notice', 'kboard')?></label>
    <?php endif?>
   </div>
  </div>
  
  <?php if($board->use_category):?>
   <?php if($board->initCategory1()):?>
   <div class="kboard-attr-row">
    <label class="attr-name" for="kboard-select-category1"><?php echo __('Category', 'kboard')?>1</label>
    <div class="attr-value">
     <select id="kboard-select-category1" name="category1">
      <option value=""><?php echo __('Category', 'kboard')?> <?php echo __('Select', 'kboard')?></option>
      <?php while($board->hasNextCategory()):?>
      <option value="<?php echo $board->currentCategory()?>"<?php if($content->category1 == $board->currentCategory()):?> selected<?php endif?>><?php echo $board->currentCategory()?></option>
      <?php endwhile?>
     </select>
    </div>
   </div>
   <?php endif?>
   
   <?php if($board->initCategory2()):?>
   <div class="kboard-attr-row">
    <label class="attr-name" for="kboard-select-category2"><?php echo __('Category', 'kboard')?>2</label>
    <div class="attr-value">
     <select id="kboard-select-category2" name="category2">
      <option value=""><?php echo __('Category', 'kboard')?> <?php echo __('Select', 'kboard')?></option>
      <?php while($board->hasNextCategory()):?>
      <option value="<?php echo $board->currentCategory()?>"<?php if($content->category2 == $board->currentCategory()):?> selected<?php endif?>><?php echo $board->currentCategory()?></option>
      <?php endwhile?>
     </select>
    </div>
   </div>
   <?php endif?>
  <?php endif?>
  
  <?php if($board->viewUsernameField()):?>
  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-member-display"><?php echo __('Author', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-member-display" name="member_display" value="<?php echo $content->member_display?>" placeholder="<?php echo __('Author', 'kboard')?>"></div>
  </div>


  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-password"><?php echo __('Password', 'kboard')?></label>
   <div class="attr-value"><input type="password" id="kboard-input-password" name="password" value="<?php echo $content->password?>" placeholder="<?php echo __('Password', 'kboard')?>"></div>
  </div>
  <?php else:?>

<input style="display:none" type="text" name="fake-autofill-fields">
  <input style="display:none" type="password" name="fake-autofill-fields">


  <!-- 비밀글 비밀번호 필드 시작 -->
  <div class="kboard-attr-row secret-password-row"<?php if(!$content->secret):?> style="display:none"<?php endif?>>
   <label class="attr-name" for="kboard-input-password"><?php echo __('Password', 'kboard')?></label>
   <div class="attr-value"><input type="password" id="kboard-input-password" name="password" value="<?php echo $content->password?>" placeholder="<?php echo __('Password', 'kboard')?>"></div>
  </div>
  <!-- 비밀글 비밀번호 필드 끝 -->
  <?php endif?>
  
  <?php if($board->useCAPTCHA() && !$content->uid):?>
   <?php if(kboard_use_recaptcha()):?>
    <div class="kboard-attr-row">
     <label class="attr-name"></label>
     <div class="attr-value"><div class="g-recaptcha" data-sitekey="<?php echo kboard_recaptcha_site_key()?>"></div></div>
    </div>
   <?php else:?>
    <div class="kboard-attr-row">
     <label class="attr-name" for="kboard-input-captcha"><img src="<?php echo kboard_captcha()?>" alt=""></label>
     <div class="attr-value"><input type="text" id="kboard-input-captcha" name="captcha" value="" placeholder="<?php echo __('CAPTCHA', 'kboard')?>"></div>
    </div>
   <?php endif?>
  <?php endif?>

  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-tel"><?php echo __('연락처', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-tel" name="kboard_option_tel" value="<?php echo $content->option->tel?>" placeholder="<?php echo __('연락처', 'kboard')?>"></div>
  </div>

  
  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-dday"><?php echo __('촬영날짜(요일)', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-dday" name="kboard_option_dday" value="<?php echo $content->option->dday?>" placeholder="<?php echo __('촬영날짜(요일)', 'kboard')?>"></div>
  </div>

    <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-stime"><?php echo __('촬영시작시간', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-stime" name="kboard_option_stime" value="<?php echo $content->option->stime?>" placeholder="<?php echo __('촬영시작시간', 'kboard')?>"></div>
  </div>
  
    <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-where"><?php echo __('촬영지역(장소)', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-where" name="kboard_option_where" value="<?php echo $content->option->where?>" placeholder="<?php echo __('촬영지역 및 장소', 'kboard')?>"></div>
  </div>
    
    <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-bname"><?php echo __('아기이름', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-bname" name="kboard_option_bname" value="<?php echo $content->option->bname?>" placeholder="<?php echo __('아기이름(영문포함)', 'kboard')?>"></div>
  </div>
    

    
<label for="fam"><font size="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;형제여부&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font></label>
<select name="kboard_option_fam" id="fam">
    <option value="">선택하세요.</option>
    <option value="첫째" <?php if ( $content->option->fam == '첫째' ) { ?>selected<?php } ?>>첫째</option>
    <option value="형제있음" <?php if ( $content->option->fam == '형제있음' ) { ?>selected<?php } ?>>형제있음</option>
    <option value="쌍둥이" <?php if ( $content->option->fam == '쌍둥이' ) { ?>selected<?php } ?>>쌍둥이</option>

</select>

<label for="cate"><font size="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;촬영선택&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font></label>
<select name="kboard_option_cate" id="cate">
    <option value="">선택하세요.</option>
    <option value="돌스냅" <?php if ( $content->option->cate == '돌스냅' ) { ?>selected<?php } ?>>돌스냅</option>
    <option value="홈스냅" <?php if ( $content->option->cate == '홈스냅' ) { ?>selected<?php } ?>>홈스냅</option>
    <option value="야외촬영" <?php if ( $content->option->cate == '야외촬영' ) { ?>selected<?php } ?>>야외촬영</option>
    <option value="가족연회" <?php if ( $content->option->cate == '가족연회' ) { ?>selected<?php } ?>>가족연회</option>
</select>

<label for="product"><font size="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;상품선택&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font></label>
<select name="kboard_option_product" id="product">
    <option value="">선택하세요.</option>
    <option value="베이직" <?php if ( $content->option->product == '베이직' ) { ?>selected<?php } ?>>베이직</option>
    <option value="프리미엄" <?php if ( $content->option->product == '프리미엄' ) { ?>selected<?php } ?>>프리미엄</option>

</select>


  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-deposit"><?php echo __('입금자명', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-deposit" name="kboard_option_deposit" value="<?php echo $content->option->deposit?>" placeholder="<?php echo __('예약금 입금자명', 'kboard')?>"> <font size="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[ 예약금계좌 : 국민은행 649702-01-318851 박용기
(10만원) ]</font> </div>
  </div>

 

  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-emali"><?php echo __('E-mail', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-email" name="kboard_option_email" value="<?php echo $content->option->email?>" placeholder="<?php echo __('E-mail', 'kboard')?>"></div>
  </div>


  
  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-option-way"><?php echo __('알게된 경로', 'kboard')?></label>
   <div class="attr-value"><input type="text" id="kboard-input-option-way" name="kboard_option_way" value="<?php echo $content->option->way?>" placeholder="<?php echo __('알게된 경로', 'kboard')?>"></div>
  </div>

<font size="2"><br><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * 촬영한 사진 중 일부는 홈페이지를 통해 공개됩니다.<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * steelpic과 함께 한 사진을 많은 분이 공감할 수 있도록 노력합니다.<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * 추가 문의사항은 반드시 새로운 글로 작성바랍니다. 기존 게시글에 댓글로 작성하실 경우 확인되지 않을 수 있습니다.<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * 쌍둥이의 경우 5만원이 추가됩니다.<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * 변경 및 취소로 인한 예약금 환불은 입금 후 [7일 이내] 에만 가능합니다.<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * 게시판의 양식에 맞게 작성해주셔야 정확한 답변을 드릴수 있습니다.<br>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; * 촬영 3일 전에 안내 전화 드립니다.<br><br>

</font>

  <!--!!!!!!!새로운부분 end!!!!!!!-->


  <div class="kboard-content">

   <?php if($board->use_editor):?>
    <?php wp_editor($content->content, 'kboard_content', array('media_buttons'=>$board->isAdmin(), 'editor_height'=>400))?>
   <?php else:?>
    <textarea name="kboard_content" id="kboard_content"><?php echo $content->content?></textarea>
   <?php endif?>
  </div>
  
<!--- 첨부파일 썸네일 추가 시작

  <div class="kboard-attr-row">
   <label class="attr-name"><?php echo __('Photos', 'kboard')?></label>
   <div class="attr-value">
    <a href="#" onclick="kboard_editor_open_media();return false;"><?php echo __('KBoard Add Media', 'kboard')?></a>
   </div>
  </div>
  
  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-input-thumbnail"><?php echo __('Thumbnail', 'kboard')?></label>
   <div class="attr-value">
    <?php if($content->thumbnail_file):?><?php echo $content->thumbnail_name?> - <a href="<?php echo $url->getDeleteURLWithAttach($content->uid);?>" onclick="return confirm('<?php echo __('Are you sure you want to delete?', 'kboard')?>');"><?php echo __('Delete file', 'kboard')?></a><?php endif?>
    <input type="file" id="kboard-input-thumbnail" name="thumbnail">
   </div>
  </div>


  <?php if($board->meta->max_attached_count > 0):?>
   
   <?php for($attached_index=1; $attached_index<=$board->meta->max_attached_count; $attached_index++):?>
   <div class="kboard-attr-row">
    <label class="attr-name" for="kboard-input-file<?php echo $attached_index?>"><?php echo __('Attachment', 'kboard')?><?php echo $attached_index?></label>
    <div class="attr-value">
     <?php if(isset($content->attach->{"file{$attached_index}"})):?><?php echo $content->attach->{"file{$attached_index}"}[1]?> - <a href="<?php echo $url->getDeleteURLWithAttach($content->uid, "file{$attached_index}");?>" onclick="return confirm('<?php echo __('Are you sure you want to delete?', 'kboard')?>');"><?php echo __('Delete file', 'kboard')?></a><?php endif?>
     <input type="file" id="kboard-input-file<?php echo $attached_index?>" name="kboard_attach_file<?php echo $attached_index?>">
    </div>
   </div>
   <?php endfor?>
   
  <?php endif?>

 
  <div class="kboard-attr-row">
   <label class="attr-name" for="kboard-select-wordpress-search"><?php echo __('WP Search', 'kboard')?></label>
   <div class="attr-value">
    <select id="kboard-select-wordpress-search" name="wordpress_search">
     <option value="1"<?php if($content->search == '1'):?> selected<?php endif?>><?php echo __('Public', 'kboard')?></option>
     <option value="2"<?php if($content->search == '2'):?> selected<?php endif?>><?php echo __('Only title (secret document)', 'kboard')?></option>
     <option value="3"<?php if($content->search == '3'):?> selected<?php endif?>><?php echo __('Exclusion', 'kboard')?></option>
    </select>
   </div>
  </div>
  
첨부파일 썸네일 추가 끝 -->

  <div class="kboard-control">
   <div class="left">
    <?php if($content->uid):?>
    <a href="<?php echo $url->set('uid', $content->uid)->set('mod', 'document')->toString()?>" class="kboard-customer-button-small"><?php echo __('Back', 'kboard')?></a>
    <a href="<?php echo $url->set('mod', 'list')->toString()?>" class="kboard-customer-button-small"><?php echo __('List', 'kboard')?></a>
    <?php else:?>
    <a href="<?php echo $url->set('mod', 'list')->toString()?>" class="kboard-customer-button-small"><?php echo __('Back', 'kboard')?></a>
    <?php endif?>
   </div>
   <div class="right">
    <?php if($board->isWriter()):?>
    <button type="submit" class="kboard-customer-button-small"><?php echo __('Save', 'kboard')?></button>
    <?php endif?>
   </div>
  </div>
 </form>
</div>

<script type="text/javascript" src="<?php echo $skin_path?>/script.js?<?php echo KBOARD_VERSION?>"></script>