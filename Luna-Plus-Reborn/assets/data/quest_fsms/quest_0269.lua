--[[
  Quest 269: [Warrior Job Change] Compensation (Mace)
  Level Required: 20
  NPC Start: 1, NPC Complete: 1
  Prerequisites: [54]
  Rewards: EXP=4149, Gold=550
]]

local fsm = require('fsm_engine')

local quest_269 = fsm:new({
    id = 269,
    name = "[Warrior Job Change] Compensation (Mace)",
    level_required = 20,
    npc_start = 1,
    npc_complete = 1,
    prerequisites = {54},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 269: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 269: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 269: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 269: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 269: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 269: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 269: Rewards given")
                self:give_rewards({
                    exp = 4149,
                    gold = 550,
                    items = {
                        { item_id = 11000160, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_269