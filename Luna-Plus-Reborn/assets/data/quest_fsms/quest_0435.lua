--[[
  Quest 435: [Quest] The choice of a gourmet
  Level Required: 52
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [434]
  Rewards: EXP=277967, Gold=60200
]]

local fsm = require('fsm_engine')

local quest_435 = fsm:new({
    id = 435,
    name = "[Quest] The choice of a gourmet",
    level_required = 52,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {434},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 435: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 435: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 435: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 285,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 435: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 435: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 435: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 435: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 435: Rewards given")
                self:give_rewards({
                    exp = 277967,
                    gold = 60200,
                    items = {
                        { item_id = 315, count = 5 },
                        { item_id = 21000010, count = 20 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_435