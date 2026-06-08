--[[
  Quest 53: [Warrior Job Change] An Old Soldier's Memory
  Level Required: 20
  NPC Start: 5, NPC Complete: 19
  Prerequisites: [46]
  Rewards: EXP=468, Gold=550
]]

local fsm = require('fsm_engine')

local quest_53 = fsm:new({
    id = 53,
    name = "[Warrior Job Change] An Old Soldier's Memory",
    level_required = 20,
    npc_start = 5,
    npc_complete = 19,
    prerequisites = {46},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 53: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 53: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 53: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 53: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 53: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 53: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 53: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 53: Rewards given")
                self:give_rewards({
                    exp = 468,
                    gold = 550,
                })
            end,
            transitions = {},
        },
    },
})

return quest_53