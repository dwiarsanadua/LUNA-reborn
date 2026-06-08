--[[
  Quest 434: High Grade Luxurious Feathers
  Level Required: 51
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [433]
  Rewards: EXP=304322, Gold=50280
]]

local fsm = require('fsm_engine')

local quest_434 = fsm:new({
    id = 434,
    name = "High Grade Luxurious Feathers",
    level_required = 51,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {433},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 434: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 434: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 434: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 434: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 434: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 434: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 434: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 434: Rewards given")
                self:give_rewards({
                    exp = 304322,
                    gold = 50280,
                    items = {
                        { item_id = 314, count = 1 },
                        { item_id = 314, count = 40 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_434