--[[
  Quest 32: Lamia Hunting
  Level Required: 38
  NPC Start: 33, NPC Complete: 33
  Rewards: EXP=76550, Gold=3350
]]

local fsm = require('fsm_engine')

local quest_32 = fsm:new({
    id = 32,
    name = "Lamia Hunting",
    level_required = 38,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 32: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 32: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 32: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 32: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 32: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 32: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 32: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 32: Rewards given")
                self:give_rewards({
                    exp = 76550,
                    gold = 3350,
                    items = {
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_32