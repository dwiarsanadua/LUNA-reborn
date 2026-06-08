--[[
  Quest 226: [Party] Continuing Threat: Kuikui
  Level Required: 30
  NPC Start: 64, NPC Complete: 65
  Prerequisites: [224]
  Rewards: EXP=37033, Gold=5320
]]

local fsm = require('fsm_engine')

local quest_226 = fsm:new({
    id = 226,
    name = "[Party] Continuing Threat: Kuikui",
    level_required = 30,
    npc_start = 64,
    npc_complete = 65,
    prerequisites = {224},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 226: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 226: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 226: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 135,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 226: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 15,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 226: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 226: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 226: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 226: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 226: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 226: Rewards given")
                self:give_rewards({
                    exp = 37033,
                    gold = 5320,
                    items = {
                        { item_id = 146, count = 1 },
                        { item_id = 150, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_226