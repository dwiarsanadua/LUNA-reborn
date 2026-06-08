--[[
  Quest 772: Conservative Tent
  Level Required: 32
  NPC Start: 28, NPC Complete: 28
  Prerequisites: [771]
  Rewards: EXP=39553, Gold=8704
]]

local fsm = require('fsm_engine')

local quest_772 = fsm:new({
    id = 772,
    name = "Conservative Tent",
    level_required = 32,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {771},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 772: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 772: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 772: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 772: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 772: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 772: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 772: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 772: Rewards given")
                self:give_rewards({
                    exp = 39553,
                    gold = 8704,
                    items = {
                        { item_id = 379, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_772