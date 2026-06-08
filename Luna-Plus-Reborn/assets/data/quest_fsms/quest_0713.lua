--[[
  Quest 713: Inappropriate Material
  Level Required: 65
  NPC Start: 46, NPC Complete: 46
  Prerequisites: [712]
  Rewards: EXP=987718
]]

local fsm = require('fsm_engine')

local quest_713 = fsm:new({
    id = 713,
    name = "Inappropriate Material",
    level_required = 65,
    npc_start = 46,
    npc_complete = 46,
    prerequisites = {712},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 713: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 713: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 713: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 713: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 713: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 713: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 713: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 713: Rewards given")
                self:give_rewards({
                    exp = 987718,
                    items = {
                        { item_id = 358, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_713