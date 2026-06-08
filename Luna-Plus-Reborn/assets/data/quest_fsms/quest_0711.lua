--[[
  Quest 711: Excellent Adhesive
  Level Required: 65
  NPC Start: 45, NPC Complete: 45
  Prerequisites: [710]
  Rewards: EXP=987718
]]

local fsm = require('fsm_engine')

local quest_711 = fsm:new({
    id = 711,
    name = "Excellent Adhesive",
    level_required = 65,
    npc_start = 45,
    npc_complete = 45,
    prerequisites = {710},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 711: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 711: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 711: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 711: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 711: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 711: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 711: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 711: Rewards given")
                self:give_rewards({
                    exp = 987718,
                    items = {
                        { item_id = 356, count = 1 },
                        { item_id = 356, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_711