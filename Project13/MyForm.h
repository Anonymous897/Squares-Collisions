#pragma once
#include "Physics.h"

namespace Project13 {
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Threading;
    using namespace Physics;

    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void) {
            InitializeComponent();
            rnd = gcnew Random();
            this->DoubleBuffered = true;
        }

    protected:
        ~MyForm() {
            if (components) delete components;
        }

    private:
        System::Windows::Forms::Button^ button1;
        System::Windows::Forms::Button^ button2;
        System::ComponentModel::Container^ components;
        Random^ rnd;

        static const int PARTICLE_COUNT = 12;
        static const int PARTICLE_SIZE = 48;
        static const double TIME_STEP = 0.5;
        static const int SUBSTEPS = 4;

        array<Panel^>^ squares;
        array<Particle^>^ particles;
        array<Thread^>^ threads;
        array<bool>^ wasReflecting;
        bool isRunning;

        int cursorX;


           int cursorY;

        delegate void AddSquareDel(int idx);
        delegate void MoveSquareDel(int idx, int x, int y);
        delegate void ColorSquareDel(int idx, int r, int g, int b);

        void AddSquare(int idx) {
            if (squares[idx] != nullptr && !Controls->Contains(squares[idx])) {
                Controls->Add(squares[idx]);
            }
        }

        void MoveSquare(int idx, int x, int y) {
            if (squares[idx] != nullptr) {
                squares[idx]->Location = Point(x, y);
            }
        }

        void ColorSquare(int idx, int r, int g, int b) {
            if (squares[idx] != nullptr) {
                squares[idx]->BackColor = Color::FromArgb(r, g, b);
            }
        }

#pragma region Windows Form Designer generated code
        void InitializeComponent(void) {
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->button2 = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(33, 30);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(86, 34);
            this->button1->TabIndex = 0;
            this->button1->Text = L"Start";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &MyForm::OnStart);
            // 
            // button2
            // 
            this->button2->Location = System::Drawing::Point(145, 30);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(86, 34);
            this->button2->TabIndex = 1;
            this->button2->Text = L"Stop";
            this->button2->UseVisualStyleBackColor = true;
            this->button2->Click += gcnew System::EventHandler(this, &MyForm::OnStop);
            // 
            // MyForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(823, 535);
            this->Controls->Add(this->button2);
            this->Controls->Add(this->button1);
            this->Name = L"MyForm";
            this->Text = L"Elastic Collisions";
            this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::OnClosing);
            this->Load += gcnew System::EventHandler(this, &MyForm::OnLoad);
            this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::OnMouseMove);
            this->ResumeLayout(false);

        }
#pragma endregion

        void ParticleThread(Object^ indexObj) {
            int idx = safe_cast<int>(indexObj);

            double startX = rnd->Next(50, ClientSize.Width - PARTICLE_SIZE - 50);
            double startY = rnd->Next(80, ClientSize.Height - PARTICLE_SIZE - 50);

            double speed = rnd->NextDouble() * 5.0 + 3.0;
            double angle = rnd->NextDouble() * 2.0 * Math::PI;
            double vx = speed * cos(angle);
            double vy = speed * sin(angle);

            double mass = rnd->NextDouble() * 3.0 + 1.0;

            particles[idx] = gcnew Particle(startX, startY, vx, vy, mass, PARTICLE_SIZE);

            int red = rnd->Next(100, 256);
            int green = rnd->Next(100, 256);
            int blue = rnd->Next(100, 256);
            squares[idx] = gcnew Panel();
            squares[idx]->Size = Drawing::Size(PARTICLE_SIZE, PARTICLE_SIZE);
            squares[idx]->Location = Point((int)startX, (int)startY);
            squares[idx]->BackColor = Color::FromArgb(red, green, blue);

            BeginInvoke(gcnew AddSquareDel(this, &MyForm::AddSquare), idx);

            double subDt = TIME_STEP / SUBSTEPS;

            while (isRunning) {
                for (int step = 0; step < SUBSTEPS; step++) {
                    PhysicsEngine::UpdatePosition(particles[idx], subDt);

                    PhysicsEngine::WallCollision(particles[idx],
                        0, 0, ClientSize.Width, ClientSize.Height);

                    for (int j = 0; j < PARTICLE_COUNT; j++) {
                        if (j == idx || particles[j] == nullptr) continue;

                        if (PhysicsEngine::CheckOverlap(particles[idx], particles[j])) {
                            PhysicsEngine::ElasticCollision(particles[idx], particles[j]);

                            int nr1 = rnd->Next(100, 256);
                            int ng1 = rnd->Next(100, 256);
                            int nb1 = rnd->Next(100, 256);
                            int nr2 = rnd->Next(100, 256);
                            int ng2 = rnd->Next(100, 256);
                            int nb2 = rnd->Next(100, 256);
                            BeginInvoke(gcnew ColorSquareDel(this, &MyForm::ColorSquare),
                                idx, nr1, ng1, nb1);
                            BeginInvoke(gcnew ColorSquareDel(this, &MyForm::ColorSquare),
                                j, nr2, ng2, nb2);
                        }
                    }
                }

                bool cursorOver = PhysicsEngine::IsCursorOver(
                    particles[idx], cursorX, cursorY);

                if (cursorOver && !wasReflecting[idx]) {
                    PhysicsEngine::CursorCollision(particles[idx], cursorX, cursorY);
                    wasReflecting[idx] = true;

                    int nr = rnd->Next(100, 256);
                    int ng = rnd->Next(100, 256);
                    int nb = rnd->Next(100, 256);
                    BeginInvoke(gcnew ColorSquareDel(this, &MyForm::ColorSquare),
                        idx, nr, ng, nb);
                }
                else if (!cursorOver) {
                    wasReflecting[idx] = false;
                }

                int px = (int)particles[idx]->posX;
                int py = (int)particles[idx]->posY;
                BeginInvoke(gcnew MoveSquareDel(this, &MyForm::MoveSquare),
                    idx, px, py);

                Thread::Sleep(16);
            }
        }

        System::Void OnStart(Object^ sender, EventArgs^ e) {
            if (isRunning) return;
            isRunning = true;

            squares = gcnew array<Panel^>(PARTICLE_COUNT);
            particles = gcnew array<Particle^>(PARTICLE_COUNT);
            threads = gcnew array<Thread^>(PARTICLE_COUNT);
            wasReflecting = gcnew array<bool>(PARTICLE_COUNT);

            for (int i = 0; i < PARTICLE_COUNT; i++) {
                wasReflecting[i] = false;
                threads[i] = gcnew Thread(
                    gcnew ParameterizedThreadStart(this, &MyForm::ParticleThread));
                threads[i]->Start(i);
            }
        }

        System::Void OnStop(Object^ sender, EventArgs^ e) {
            isRunning = false;

            if (threads != nullptr) {
                for (int i = 0; i < threads->Length; i++) {
                    if (threads[i] != nullptr && threads[i]->IsAlive)
                        threads[i]->Join(500);
                }
            }

            if (squares != nullptr) {
                for (int i = 0; i < PARTICLE_COUNT; i++) {
                    if (squares[i] != nullptr) {
                        Controls->Remove(squares[i]);
                        delete squares[i];
                        squares[i] = nullptr;
                    }
                }
            }
        }

        System::Void OnClosing(Object^ sender, FormClosingEventArgs^ e) {
            isRunning = false;
            if (threads != nullptr) {
                for (int i = 0; i < threads->Length; i++) {
                    if (threads[i] != nullptr && threads[i]->IsAlive)
                        threads[i]->Join(1000);
                }
            }
        }

        System::Void OnLoad(Object^ sender, EventArgs^ e) {
            this->BackColor = Color::White;
            this->Text = L"Elastic Collisions — move mouse to interact";
        }

        System::Void OnMouseMove(Object^ sender, MouseEventArgs^ e) {
            cursorX = e->X;
            cursorY = e->Y;
        }
    };
}