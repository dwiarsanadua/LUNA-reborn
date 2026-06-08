--[[
  Quest 414: [Quest] The Perpetrator of Kierra
  Level Required: 78
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [413]
  Rewards: EXP=1850968, Gold=168504
]]

local fsm = require('fsm_engine')

local quest_414 = fsm:new({
    id = 414,
    name = "[Quest] The Perpetrator of Kierra",
    level_required = 78,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {413},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 414: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 414: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 414: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 73,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 414: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 414: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 414: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 414: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 414: Rewards given")
                self:give_rewards({
                    exp = 1850968,
                    gold = 168504,
                })
            end,
            transitions = {},
        },
    },
})

return quest_414