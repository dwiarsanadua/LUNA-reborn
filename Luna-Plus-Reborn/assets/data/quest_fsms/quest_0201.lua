--[[
  Quest 201: Dangerous Experimental Materials
  Level Required: 46
  NPC Start: 7, NPC Complete: 7
  Rewards: EXP=170370, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_201 = fsm:new({
    id = 201,
    name = "Dangerous Experimental Materials",
    level_required = 46,
    npc_start = 7,
    npc_complete = 7,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 201: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 201: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 201: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 201: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 201: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 201: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 201: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 201: Rewards given")
                self:give_rewards({
                    exp = 170370,
                    gold = 4500,
                    items = {
                        { item_id = 138, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_201