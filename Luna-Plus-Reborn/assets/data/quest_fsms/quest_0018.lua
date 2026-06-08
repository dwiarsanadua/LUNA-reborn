--[[
  Quest 18: Goblin Rodeo
  Level Required: 19
  NPC Start: 21, NPC Complete: 21
  Prerequisites: [17]
  Rewards: EXP=5735, Gold=1960
]]

local fsm = require('fsm_engine')

local quest_18 = fsm:new({
    id = 18,
    name = "Goblin Rodeo",
    level_required = 19,
    npc_start = 21,
    npc_complete = 21,
    prerequisites = {17},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 18: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 18: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 18: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 33,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 18: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 18: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 18: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 18: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 18: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 18: Rewards given")
                self:give_rewards({
                    exp = 5735,
                    gold = 1960,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_18