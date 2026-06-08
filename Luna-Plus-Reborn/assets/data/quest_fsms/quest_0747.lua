--[[
  Quest 747: [Quest] Pope Street Congestion
  Level Required: 69
  NPC Start: 117, NPC Complete: 404
  Prerequisites: [746]
  Rewards: EXP=1412170
]]

local fsm = require('fsm_engine')

local quest_747 = fsm:new({
    id = 747,
    name = "[Quest] Pope Street Congestion",
    level_required = 69,
    npc_start = 117,
    npc_complete = 404,
    prerequisites = {746},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 747: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 747: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 747: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 747: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 404,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 747: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 747: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 404,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 747: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 747: Rewards given")
                self:give_rewards({
                    exp = 1412170,
                    items = {
                        { item_id = 372, count = 1 },
                        { item_id = 372, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_747