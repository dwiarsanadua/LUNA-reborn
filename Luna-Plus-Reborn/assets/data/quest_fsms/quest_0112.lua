--[[
  Quest 112: Silly Old Bear
  Level Required: 18
  NPC Start: 57, NPC Complete: 61
  Rewards: EXP=6540, Gold=2085
]]

local fsm = require('fsm_engine')

local quest_112 = fsm:new({
    id = 112,
    name = "Silly Old Bear",
    level_required = 18,
    npc_start = 57,
    npc_complete = 61,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 112: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 112: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 112: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 112: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 112: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 112: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 112: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 61,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 112: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 112: Rewards given")
                self:give_rewards({
                    exp = 6540,
                    gold = 2085,
                    items = {
                        { item_id = 51, count = 1 },
                        { item_id = 51, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_112