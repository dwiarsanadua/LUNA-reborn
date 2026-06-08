--[[
  Quest 830: The worse spirit.
  Level Required: 77
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=1712009, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_830 = fsm:new({
    id = 830,
    name = "The worse spirit.",
    level_required = 77,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 830: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 830: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 830: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 70,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 830: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 830: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 830: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 830: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 830: Rewards given")
                self:give_rewards({
                    exp = 1712009,
                    gold = 41009,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_830