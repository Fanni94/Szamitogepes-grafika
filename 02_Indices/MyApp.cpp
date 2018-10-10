#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_programID = 0;

	lenyomva = false;
	elfedes = false;
	novekedes = 0.3f;
	startelfedesido = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//
	// geometria létrehozása
	//

	Vertex vert[] =
	{ 
		//          x,  y, z             R, G, B
		{glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0)},
		{glm::vec3( 1, -1, -1), glm::vec3(0, 1, 0)},
		{glm::vec3(-1,  1, -1), glm::vec3(0, 0, 1)},
		{glm::vec3( 1,  1, -1), glm::vec3(1, 1, 1)},
		{ glm::vec3(-1, -1, 1), glm::vec3(1, 0, 1) },
		{ glm::vec3(1, -1, 1), glm::vec3(0, 1, 0) },
		{ glm::vec3(-1,  1, 1), glm::vec3(1, 1, 0) },
		{ glm::vec3(1,  1, 1), glm::vec3(0, 1, 1) }
	};

	// indexpuffer adatai
    GLushort indices[]=
    {
		// háromszögek
		4,5,6,
		6,5,7,
		7,5,1,
		1,3,7,
		6,7,3,
		2,6,3,
		6,2,0,
		0,4,6,
		4,0,5,
		5,0,1,
		0,2,3,
		3,1,0,
		
    };

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
				  sizeof(vert),		// ennyi bájt nagyságban
				  vert,	// errõl a rendszermemóriabeli címrõl olvasva
				  GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat
	

	// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	); 

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzformációs mátrixok címének lekérdezése
	m_loc_mvp = glGetUniformLocation( m_programID, "MVP");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	// nézeti transzformáció beállítása
	//float time = SDL_GetTicks() / 1000.0;
	m_matView = glm::lookAt(glm::vec3( 1 ,  1,  10),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}


void CMyApp::Kocka(glm::mat4& transzformacio) {
	m_matWorld = glm::scale(glm::vec3(novekedes, novekedes, novekedes));
	
	glm::mat4 mvp = m_matProj * m_matView * m_matWorld * transzformacio;

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glDrawElements(GL_TRIANGLES,		// primitív típus
		36,					// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime
}

void CMyApp::Tetris(glm::mat4& transzformacio) {
	Kocka(glm::translate(transzformacio, glm::vec3(0.0f, 0.0f, 0.0f)));
	Kocka(glm::translate(transzformacio, glm::vec3(2.0f, 0.0f, 0.0f)));
	Kocka(glm::translate(transzformacio, glm::vec3(-2.0f, 0.0f, 0.0f)));
	Kocka(glm::translate(transzformacio, glm::vec3(0.0f, 2.0f, 0.0f)));
	Kocka(glm::translate(transzformacio, glm::vec3(0.0f, -2.0f, 0.0f)));
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// shader parameterek beállítása
	float sebesseg = 31.688f / 6.0f;  //arc length of  z = 0.01*x^3 + 0.05*x^2 from x=-10 to 10
	float x = fmod((SDL_GetTicks() / 1000.0) * sebesseg,  40) - 10;
	if (x > 10) {
		x = 10 - (x - 10);
	}
	glm::mat4 gorbemozg = glm::translate(glm::vec3(x, 0, 0.01*x*x*x + 0.05*x*x));

	float rezgseb = 120.0f / 0.2f;
	float time = SDL_GetTicks() / 1000.0;
	float szog = fmod((SDL_GetTicks() / 1000.0) * rezgseb, 60);
	if (szog > 15 && szog < 45) {
		szog = 15 - (szog - 15);
	}
	if (szog >= 45) {
		szog = -30 + (szog - 30);
	}

	glm::mat4 rezges = glm::rotate(0.0f, glm::vec3(1, 0, 0));
	if ((fmod(time, 5) < 1.0f && time >= 5) || elfedes == true) {
		rezges = glm::rotate(glm::radians(szog), glm::vec3(1, 0, 0));
		if (lenyomva == true) {
			elfedes = true;
			startelfedesido = time;
		}
	}

	if (elfedes == true) {
		novekedes = (time - startelfedesido) * (time - startelfedesido) + 0.3f;
		if (novekedes > 10.0f) {
			elfedes = false;
			novekedes = 0.3f;
		}
	}
	
	for (int i = 0; i < 5; i++)
	{
		Tetris(gorbemozg * glm::rotate(glm::radians(360.0f / 5.0f * i), glm::vec3(0, 0, 1)) * glm::translate(glm::vec3(5.0f, 0.0f, 0.0f)) * rezges);
	}

	lenyomva = false;
	
	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if (key.keysym.scancode == SDL_SCANCODE_SPACE) {
		lenyomva = true;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}