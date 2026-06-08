--[[
  Quest 31: Orcish Patriarch
  Level Required: 43
  NPC Start: 32, NPC Complete: 32
  Prerequisites: [28]
  Rewards: EXP=118872, Gold=10700
]]

local fsm = require('fsm_engine')

local quest_31 = fsm:new({
    id = 31,
    name = "Orcish Patriarch",
    level_required = 43,
    npc_start = 32,
    npc_complete = 32,
    prerequisites = {28},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 31: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 31: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 31: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 59,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 31: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 31: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 31: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 31: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 31: Rewards given")
                self:give_rewards({
                    exp = 118872,
                    gold = 10700,
                    items = {
                        { item_id = 21000097, count = 6 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_31