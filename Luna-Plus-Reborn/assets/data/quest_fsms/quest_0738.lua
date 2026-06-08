--[[
  Quest 738: [Quest] Distorted Truth
  Level Required: 60
  NPC Start: 9, NPC Complete: 117
  Prerequisites: [737]
  Rewards: EXP=729940
]]

local fsm = require('fsm_engine')

local quest_738 = fsm:new({
    id = 738,
    name = "[Quest] Distorted Truth",
    level_required = 60,
    npc_start = 9,
    npc_complete = 117,
    prerequisites = {737},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 738: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 738: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 738: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 738: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 738: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 738: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 738: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 738: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 738: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 738: Rewards given")
                self:give_rewards({
                    exp = 729940,
                    items = {
                        { item_id = 364, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_738